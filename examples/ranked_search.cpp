#include <trie/trie.hpp>
#include <iostream>

static void run()
{
  trie::Trie t;

  t.insert("hello");
  t.insert("hallo");
  t.insert("hullo");
  t.insert("help");
  t.insert("world");

  const auto results = t.search_ranked("helo", 3);

  std::cout << "Ranked results for 'helo':\n";
  for (const auto &word : results)
  {
    std::cout << "  " << word << "\n";
  }
}

int main()
{
  run();
  return 0;
}
