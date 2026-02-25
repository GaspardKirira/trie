#include <trie/trie.hpp>
#include <iostream>

static void run()
{
  trie::Trie t;

  t.insert("alice");
  t.insert("bob");

  std::cout << std::boolalpha;
  std::cout << "contains alice: " << t.contains("alice") << "\n";
  std::cout << "contains bob:   " << t.contains("bob") << "\n";
  std::cout << "contains eve:   " << t.contains("eve") << "\n";
}

int main()
{
  run();
  return 0;
}
