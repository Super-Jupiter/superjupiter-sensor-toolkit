namespace superjupiter {
struct Config {};
struct Result {};
namespace unrelated {
struct State {};
} // namespace unrelated
} // namespace superjupiter
#include <SuperJupiter/SensorToolkit/ChannelAlign.h>
using namespace superjupiter::channel_align;
int main() {
  ChannelAlign<2> channels(1000, 100);
  channels.push(0, 20, 0);
  channels.push(0, 22, 1000);
  Result aligned = channels.at(0, 500);
  (void)aligned;
  return 0;
}
