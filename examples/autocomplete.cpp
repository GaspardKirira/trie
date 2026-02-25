#include <trie/trie.hpp>
#include <iostream>

static void run()
{
  trie::Trie t;

  t.insert("apple");
  t.insert("app");
  t.insert("application");
  t.insert("banana");

  const auto suggestions = t.suggest("app");

  std::cout << "Suggestions for 'app':\n";
  for (const auto &word : suggestions)
  {
    std::cout << "  " << word << "\n";
  }
}

int main()
{
  run();
  return 0;
}
