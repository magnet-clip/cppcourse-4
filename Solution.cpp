#include "Common.h"

#include <map>
#include <mutex>
#include <unordered_map>

using namespace std;

class LruCache : public ICache {
public:
  LruCache(shared_ptr<IBooksUnpacker> books_unpacker, const Settings &settings)
      : _unpacker(books_unpacker), _cache({settings.max_memory}) {}

  BookPtr GetBook(const string &book_name) override {
    lock_guard guard(_cache_mutex);
    auto book = _cache.Find(book_name);
    if (!book) {
      auto unpacked_book = _unpacker->UnpackBook(book_name);
      book = shared_ptr<const IBook>(unpacked_book.release());
      _cache.Add(book);
    } else {
      _cache.Up(book);
    }
    return book;
  }

private:
  struct Cache {
    using PriorityCache = map<size_t, BookPtr>;
    using NameCache = unordered_map<string, BookPtr>;

    explicit Cache(size_t memory_limit) : _memory_limit(memory_limit) {}

    BookPtr Find(const string &name) {
      auto it = _name.find(name);
      if (it != _name.end()) {
        return it->second;
      }
      return nullptr;
    }

    void Add(const BookPtr &book) {
      _priority[_max_priority] = book;
      _name[book->GetName()] = book;
      _max_priority++;
      Cleanup();
    }

    void Up(BookPtr book) {}

  private:
    void Cleanup() {}

    PriorityCache _priority;
    NameCache _name;
    size_t _memory_limit;
    size_t _max_priority;
  };

  shared_ptr<IBooksUnpacker> _unpacker;

  mutex _cache_mutex;
  Cache _cache;
};

unique_ptr<ICache> MakeCache(shared_ptr<IBooksUnpacker> books_unpacker,
                             const ICache::Settings &settings) {
  return unique_ptr<ICache>(new LruCache(books_unpacker, settings));
}
