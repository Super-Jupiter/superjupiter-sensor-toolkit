#include <SuperJupiter/SensorToolkit/SignalFilter.h>
using namespace superjupiter::signal_filter;
int main() {
  SignalFilter filter(Config(1000, 5000));
  filter.push(20, 0);
  Result result = filter.push(21, 100);
  (void)result;
  return 0;
}
