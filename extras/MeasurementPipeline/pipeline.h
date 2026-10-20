// SPDX-License-Identifier: MIT
#ifndef SUPERJUPITER_PORTFOLIO_EXAMPLE_PIPELINE_H
#define SUPERJUPITER_PORTFOLIO_EXAMPLE_PIPELINE_H
#include <SuperJupiter/SensorToolkit/Calibration.h>
#include <SuperJupiter/SensorToolkit/ChannelAlign.h>
#include <SuperJupiter/EmbeddedReliability/ConfigStore.h>
#include <SuperJupiter/EmbeddedReliability/DeviceHealth.h>
#include <SuperJupiter/SensorToolkit/EventCapture.h>
#include <SuperJupiter/SensorToolkit/Integration.h>
#include <SuperJupiter/SensorToolkit/MachineState.h>
#include <SuperJupiter/EmbeddedReliability/utilities/PowerBudget.h>
#include <SuperJupiter/SensorToolkit/PulseMetrics.h>
#include <SuperJupiter/EmbeddedReliability/RecordJournal.h>
#include <SuperJupiter/SensorToolkit/SampleWindow.h>
#include <SuperJupiter/SensorToolkit/SensorAgreement.h>
#include <SuperJupiter/SensorToolkit/SensorQuality.h>
#include <SuperJupiter/SensorToolkit/testing/SensorReplay.h>
#include <SuperJupiter/SensorToolkit/SignalFilter.h>
#include <SuperJupiter/EmbeddedReliability/StoreForward.h>
#include <SuperJupiter/SensorToolkit/ThresholdEvents.h>
#include <SuperJupiter/SensorToolkit/TimeAggregate.h>
#include <SuperJupiter/SensorToolkit/TimeAnchor.h>
#include <SuperJupiter/SensorToolkit/Trend.h>
#include <math.h>
namespace superjupiter {
namespace portfolio_example {
// Synthetic RAM only. This does not claim persistence or hardware validation.
class Memory : public record_journal::Storage, public config_store::Storage {
  uint8_t bytes_[256];

public:
  Memory() {
    for (size_t i = 0; i < sizeof(bytes_); ++i)
      bytes_[i] = 255;
  }
  bool read(size_t offset, uint8_t *output, size_t length) {
    if (offset > sizeof(bytes_) || length > sizeof(bytes_) - offset)
      return false;
    for (size_t i = 0; i < length; ++i)
      output[i] = bytes_[offset + i];
    return true;
  }
  bool write(size_t offset, const uint8_t *input, size_t length) {
    if (offset > sizeof(bytes_) || length > sizeof(bytes_) - offset)
      return false;
    for (size_t i = 0; i < length; ++i)
      bytes_[offset + i] = input[i];
    return true;
  }
  bool sync() { return true; }
};
struct Packet {
  uint16_t valueTenths;
  uint32_t capturedAtMs;
};
struct Report {
  bool passed;
  unsigned observations, acknowledged;
  uint64_t integratedMs;
};
inline Report run() {
  Report report = {false, 0, 0, 0};
  Memory memory;
  config_store::ConfigStore<4> settings(memory, 1, 192);
  uint8_t config[4] = {100, 0, 25, 1}, loaded[4] = {};
  if (settings.load(loaded) != config_store::Status::Empty ||
      settings.save(config) != config_store::Status::Ok)
    return report;
  if (settings.load(loaded) != config_store::Status::Ok || loaded[0] != 100)
    return report;
  record_journal::RecordJournal<4, 8> journal(memory);
  if (journal.recover() != record_journal::Status::Ok)
    return report;
  store_forward::StoreForward<Packet, 4> uploads;
  sensor_quality::Config qualityConfig;
  qualityConfig.rangeEnabled = true;
  qualityConfig.minimum = -40;
  qualityConfig.maximum = 60;
  qualityConfig.expectedIntervalMs = 100;
  qualityConfig.gapToleranceMs = 20;
  sensor_quality::SensorQuality quality[2] = {
      sensor_quality::SensorQuality(qualityConfig),
      sensor_quality::SensorQuality(qualityConfig)};
  sample_window::SampleWindow<4> raw[2];
  calibration::Point points[] = {{0, 0}, {100, 100}};
  calibration::Calibration calibrate(points, 2, 1);
  time_anchor::TimeAnchor clock(time_anchor::Config(5000, 100));
  const uint32_t start = 0xffffff00UL;
  if (clock.setAnchor(start, 1700000000000LL, 5) != time_anchor::Status::Ok)
    return report;
  signal_filter::SignalFilter filters[2] = {
      signal_filter::SignalFilter(signal_filter::Config(200, 300)),
      signal_filter::SignalFilter(signal_filter::Config(200, 300))};
  trend::Trend<4> trends[2] = {trend::Trend<4>(trend::Config(2, 100, 300)),
                               trend::Trend<4>(trend::Config(2, 100, 300))};
  threshold_events::ThresholdEvents alarm(
      threshold_events::Config(24, 25, 100, 100, 300));
  pulse_metrics::PulseMetrics meter(pulse_metrics::Config(1000, 200000, 0.5));
  time_aggregate::TimeAggregate aggregate(400);
  integration::Integration integral(300);
  event_capture::EventCapture<3, 2> event;
  bool triggered = false;
  channel_align::ChannelAlign<2> aligned(300, 150);
  machine_state::MachineState machine(machine_state::Config(21, 25, 100, 300));
  device_health::DeviceHealth health(100, 20, 500);
  sensor_replay::Sample source[32];
  for (size_t i = 0; i < 32; ++i)
    source[i] = sensor_replay::Sample{
        20 + static_cast<double>(i / 2) + (i % 2 ? 0.25 : 0),
        static_cast<uint32_t>((i / 2) * 100), static_cast<uint16_t>(i % 2)};
  sensor_replay::Fault faults[] = {{10, sensor_replay::FaultKind::Replace, NAN},
                                   {17, sensor_replay::FaultKind::Drop, 0},
                                   {24, sensor_replay::FaultKind::Replace, 80}};
  sensor_replay::SensorReplay replay(source, 32, faults, 3);
  sensor_replay::Output sample = {};
  if (replay.start(start) != sensor_replay::Status::Ready)
    return report;
  bool deferredSecond = false;
  for (uint32_t elapsed = 0; elapsed <= 1500; elapsed += 100) {
    const uint32_t now = start + elapsed;
    for (;;) {
      const sensor_replay::Status next = replay.next(now, sample);
      if (next == sensor_replay::Status::Waiting ||
          next == sensor_replay::Status::Finished)
        break;
      if (next != sensor_replay::Status::SampleReady || sample.channel >= 2)
        return report;
      ++report.observations;
      const size_t channel = sample.channel;
      if (!raw[channel]
               .push(static_cast<float>(sample.raw), sample.atMs)
               .accepted)
        return report;
      const sensor_quality::Result q =
          quality[channel].observe(static_cast<float>(sample.raw), sample.atMs);
      if (q.has(sensor_quality::InvalidTimestamp) ||
          q.has(sensor_quality::InvalidConfiguration))
        return report;
      const bool usable = !q.has(sensor_quality::InvalidNumber) &&
                          !q.has(sensor_quality::OutOfRange) &&
                          !q.has(sensor_quality::NumericError);
      const time_anchor::Result utc = clock.convert(sample.atMs);
      if (utc.status != time_anchor::Status::Ok || utc.revision != 1 ||
          utc.utcMs !=
              1700000000000LL + static_cast<uint32_t>(sample.atMs - start))
        return report;
      if (usable) {
        const calibration::Result value = calibrate.evaluate(sample.raw);
        if (!value.valid())
          return report;
        if (q.has(sensor_quality::SampleGap)) {
          filters[channel].reset();
          trends[channel].reset();
        }
        const signal_filter::Result filtered =
            filters[channel].push(value.value, sample.atMs);
        if (filtered.status != signal_filter::Status::Seeded &&
            filtered.status != signal_filter::Status::Filtered &&
            filtered.status != signal_filter::Status::GapReset)
          return report;
        if (trends[channel].push(value.value, sample.atMs) != trend::Status::Ok)
          return report;
        if (aligned.push(channel, value.value, sample.atMs) !=
            channel_align::Status::Ok)
          return report;
      } else {
        filters[channel].reset();
        trends[channel].reset();
      }
      if (channel != 0)
        continue;
      if (health.recordSample(sample.atMs) != device_health::Status::Ok)
        return report;
      // Independent synthetic meter pulse. Explicit ms->us conversion, with
      // normal uint32 wrap.
      meter.pulse(
          static_cast<uint32_t>(static_cast<uint64_t>(sample.atMs) * 1000));
      event.push(sample.raw, sample.atMs);
      const double value = usable ? sample.raw : NAN;
      const threshold_events::Result state = alarm.push(value, sample.atMs);
      machine.push(value, sample.atMs);
      integral.push(value, sample.atMs);
      if (state.state == threshold_events::State::Alarm && !triggered) {
        event.trigger();
        triggered = true;
      }
      const time_aggregate::Status status = aggregate.push(value, sample.atMs);
      if (status == time_aggregate::Status::Completed) {
        time_aggregate::Summary summary;
        if (!aggregate.takeCompleted(summary) || summary.numericError ||
            !summary.finiteCount)
          return report;
        const uint16_t encoded = static_cast<uint16_t>(summary.mean * 10 + 0.5);
        const uint8_t wire[4] = {static_cast<uint8_t>(encoded),
                                 static_cast<uint8_t>(encoded >> 8),
                                 static_cast<uint8_t>(journal.size() + 1), 1};
        if (journal.append(wire) != record_journal::Status::Ok)
          return report;
        Packet packet = {encoded, sample.atMs};
        if (uploads.enqueue(journal.size(), packet) !=
            store_forward::Status::Ok)
          return report;
      } else if (status != time_aggregate::Status::Accepted)
        return report;
      health.observeQueue(uploads.size());
    }
    uint64_t id = 0;
    Packet packet = {};
    const store_forward::Status reserved = uploads.reserve(now, id, packet);
    if (reserved == store_forward::Status::Ok) {
      if (id == 2 && !deferredSecond) {
        deferredSecond = true;
        if (uploads.fail(id, now, 100) != store_forward::Status::Ok)
          return report;
      } else {
        if (!packet.valueTenths ||
            uploads.acknowledge(id) != store_forward::Status::Ok)
          return report;
        ++report.acknowledged;
      }
    } else if (reserved != store_forward::Status::Empty &&
               reserved != store_forward::Status::Waiting)
      return report;
  }
  const channel_align::Result left = aligned.at(0, start + 1500),
                              right = aligned.at(1, start + 1500);
  if (left.status != channel_align::Status::Ok ||
      right.status != channel_align::Status::Ok)
    return report;
  sensor_agreement::Sample pair[] = {{left.value, 0}, {right.value, 0}};
  if (sensor_agreement::compare(pair, 2, sensor_agreement::Config(0.5))
          .status != sensor_agreement::Status::Agree)
    return report;
  power_budget::State profile[4];
  for (unsigned i = 0; i < 4; ++i)
    profile[i] = power_budget::State{
        i == 1 ? 1.0 : 100.0, static_cast<uint32_t>(machine.durationMs(
                                  static_cast<machine_state::State>(i)))};
  const power_budget::Result battery =
      power_budget::estimate(profile, 4, 1000, 0.8, 0.2);
  if (battery.status != power_budget::Status::Ok || battery.runtimeHours <= 0)
    return report;
  record_journal::RecordJournal<4, 8> restored(memory);
  uint8_t first[4] = {};
  if (restored.recover() != record_journal::Status::Ok ||
      restored.size() != 3 ||
      restored.get(0, first) != record_journal::Status::Ok || first[2] != 1)
    return report;
  event_capture::Sample firstEvent = {};
  if (!event.complete() || !event.get(0, firstEvent) ||
      firstEvent.raw == firstEvent.raw)
    return report;
  const trend::Result trend = trends[0].summary();
  if (trend.status != trend::Status::Ok ||
      ::fabs(trend.slopePerSecond - 10) > 0.01)
    return report;
  const device_health::Snapshot diagnostics = health.snapshot(start + 1500);
  report.integratedMs = integral.summary().coveredMs;
  report.passed = report.observations == 31 && report.acknowledged == 3 &&
                  uploads.size() == 0 && raw[0].summary().maximum == 80 &&
                  raw[0].count() == 4 && raw[1].count() == 4 &&
                  diagnostics.samples == 16 && diagnostics.gaps == 0 &&
                  meter.poll(static_cast<uint32_t>(
                                 static_cast<uint64_t>(start + 1500) * 1000))
                          .count == 16 &&
                  report.integratedMs == 1100 &&
                  ::fabs(integral.summary().valueSeconds - 29.85) < 0.01 &&
                  machine.durationMs(machine_state::State::Running) > 0;
  return report;
}
} // namespace portfolio_example
} // namespace superjupiter
#endif
