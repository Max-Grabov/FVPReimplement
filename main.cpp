#include "fvp.hpp"

int main(int, char **)
{
  fvp::Core::FVP app{};
  app.Run();
}
