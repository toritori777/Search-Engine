#include "./WordIndex.h"
#include <iostream>
#include <bits/stdc++.h>
#include <chrono>
#include <set>
using namespace std::chrono;
using namespace std;


namespace searchserver {

WordIndex::WordIndex() {
  // TODO: implement
  unordered_map<string, unordered_map<string, int>> index_;
}

size_t WordIndex::num_words() {
  // TODO: implement
  return index_.size();
}

void WordIndex::record(const string& word, const string& doc_name) {
  if (index_.find(word) != index_.end()) {
    if (index_[word].count(doc_name) > 0) {
      index_[word][doc_name]++;
    } else {
      index_[word][doc_name] = 1;
    }
  } else {
    unordered_map<string, int> new_doc;
    new_doc[doc_name] = 1;
    index_[word] = new_doc;
  }
}

bool cmp(Result a, Result b)
{
    return a.rank > b.rank;
}

list<Result> WordIndex::lookup_word(const string& word) {
  list<Result> result;
  // TODO: implement
  unordered_map<string, int> wordValue = index_[word];
  unordered_map<string, int>::iterator itr;
  for (itr = wordValue.begin(); itr != wordValue.end(); itr++) {
    result.push_back(Result(itr->first, itr->second));
  }
  result.sort(cmp);
  return result;
}

list<Result> WordIndex::lookup_query(const vector<string>& query) {
  list<Result> results;
  // TODO: implement
  unordered_map<string, int> results_map; 
  unordered_set<string> valid_docs;
  if (query.size() == 0) {
    return results;
  }
  for (auto& v : index_) {
    auto& freq_map = v.second;
    for (auto& kv : freq_map) {
      valid_docs.insert(kv.first);
    }
  }
  bool flag = false;
  for (auto& word : query) {
    if (index_.find(word) != index_.end()) {
      flag = true;
      auto& freq_map = index_[word];
      unordered_set<string> temp;
      for (auto& kv: freq_map) {
        temp.insert(kv.first);
      }
      unordered_set<string> to_del;
      for (auto& s : valid_docs) {
        if (temp.find (s) == temp.end()) {  
          to_del.insert(s);
        }
      }
      // delete
      for (auto& s: to_del) {
        valid_docs.erase(s);
      }
    }
    else {
      valid_docs.clear();
      break;
    }
  }
  if (flag == false) {
    valid_docs.clear();
  }
  if (valid_docs.size() == 0) {
    return results;
  }
  for (string q: query) {
    if (index_.find(q) != index_.end()) {
      unordered_map<string, int> wordValue = index_[q];
      unordered_map<string, int>::iterator itr;
      for (itr = wordValue.begin(); itr != wordValue.end(); itr++) {
        if (valid_docs.find(itr->first) != valid_docs.end()) {   
          bool find = false;
          if (results_map.find(itr->first) != results_map.end()) {
            find = true;
            results_map[itr->first] += itr->second;
          }
          if (find == false) {
            results_map.insert(make_pair(itr->first, itr->second));
          } 
        }
      }
    }
  }
  unordered_map<string, int>::iterator it;
  for (it = results_map.begin(); it != results_map.end(); it++)
  {
    results.push_back(Result(it->first, it->second));
  }
  results.sort(cmp);
  return results;
}

}  // namespace searchserver
