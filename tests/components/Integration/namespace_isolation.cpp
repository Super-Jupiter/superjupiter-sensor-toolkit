namespace superjupiter {
struct Config {};
struct Result {};
namespace unrelated {
struct State {};
} // namespace unrelated
} // namespace superjupiter
#include <SuperJupiter/SensorToolkit/Integration.h>
using namespace superjupiter::integration;
int main() {
  Integration energy(5000);
  energy.push(100, 0);
  Result joules = energy.push(100, 1000);
  (void)joules;
  return 0;
}
