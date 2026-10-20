#include <SuperJupiter/SensorToolkit/TimeAnchor.h>
using namespace superjupiter::time_anchor;
int main() {
  TimeAnchor clock(Config(3600000, 100));
  clock.setAnchor(1000, 1700000000000LL, 20);
  Result result = clock.convert(1500);
  (void)result;
  return 0;
}
