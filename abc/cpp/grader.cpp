#include "abc.h"
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
using namespace std;

const int MAX_LA = 100000;
const int MAX_LB = 100000;
// const int MAX_L = 20000000;
const int MAX_L = 100000000;

void fatal(const string& msg) {
  cerr << msg << endl;
  exit(1);
}

void ensureImpl(bool condition, const char* conditionStr) {
  if (condition)
    return;
  fatal("Condition does not satisfy: " + string(conditionStr));
}

#define ensure(x...) ensureImpl(x, #x)

template <typename F>
void timed(const string& name, const F& func) {
  cerr << "Running " << name << "..." << endl;
  auto start = chrono::high_resolution_clock::now();
  func();
  auto end = chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
  cerr << name << " took " << fixed << setprecision(6) << (double) duration.count() / 1e9 << " seconds" << endl;
}

int main() {
  freopen("abc.log", "w", stderr);
  // ==================== Read Input ====================
  int n, m;
  cin >> n >> m;
  ensure(0 <= n && n <= 700);
  ensure(0 <= m && m <= 1000);

  auto readUshort = [&]() {
    string s;
    cin >> s;
    stringstream ss(s);
    unsigned short x;
    ss >> x;
    if (s != to_string(x))
      fatal("Invalid input for unsigned short: " + s);
    return x;
  };
  vector<pair<string, unsigned short>> students;
  map<string, unsigned short> studentsMap;
  for (int i = 0; i < n; i++) {
    string name;
    cin >> name;
    if (name.empty())
      fatal("Expected name");
    if (name.size() > 4)
      fatal("Name too long: " + name);
    bool ok = true;
    for (char c : name)
      if (c < 'a' || c > 'z')
        ok = false;
    if (!ok)
      fatal("Invalid name: " + name);
    unsigned short number = readUshort();
    students.emplace_back(name, number);

    if (studentsMap.count(name))
      fatal("Duplicate name: " + name);
    studentsMap[name] = number;
  }

  auto readExistingName = [&] {
    string name;
    cin >> name;
    if (!studentsMap.count(name))
      fatal("Unknown name in Bob's input: " + name);
    return name;
  };
  vector<pair<string, string>> messages;
  for (int i = 0; i < m; i++) {
    string sender = readExistingName();
    string recipient = readExistingName();
    messages.emplace_back(sender, recipient);
  }

  // ==================== Call Alice ====================
  char (*aliceNames)[5] = new char[n][5];
  memset(aliceNames[0], 0, n * 5);
  unsigned short* aliceNumbers = new unsigned short[n];
  bool* aliceOutputs = new bool[MAX_LA];
  memset(aliceOutputs, 0, MAX_LA);
  for (int i = 0; i < n; i++) {
    strcpy(aliceNames[i], students[i].first.c_str());
    aliceNumbers[i] = students[i].second;
  }
  int la;
  timed("Alice", [&]() {
    la = alice(n, aliceNames, aliceNumbers, aliceOutputs);
  });
  cerr << "la = " << la << endl;
  ensure(0 <= la && la <= MAX_LA);

  // ==================== Call Bob ====================
  char (*bobSenders)[5] = new char[m][5];
  memset(bobSenders[0], 0, m * 5);
  char (*bobRecipients)[5] = new char[m][5];
  memset(bobRecipients[0], 0, m * 5);
  bool* bobOutputs = new bool[MAX_LB];
  memset(bobOutputs, 0, MAX_LB);
  for (int i = 0; i < m; i++) {
    strcpy(bobSenders[i], messages[i].first.c_str());
    strcpy(bobRecipients[i], messages[i].second.c_str());
  }
  int lb;
  timed("Bob", [&]() {
    lb = bob(m, bobSenders, bobRecipients, bobOutputs);
  });
  cerr << "lb = " << lb << endl;
  ensure(0 <= lb && lb <= MAX_LB);

  // ==================== Debug ====================
  
  /*
  // system("pause");
  cout << la << ":\n";
  for(int i=0; i<la;) {
    for(int j=0; j<21; i++, j++)
      cout << aliceOutputs[i];
    cout << endl;
    for(int j=0; j<21; i++, j++)
      cout << aliceOutputs[i];
    cout << endl;
    for(int j=0; j<16; i++, j++)
      cout << aliceOutputs[i];
    cout << endl << endl;
  }
  cout << endl << endl << endl;
  cout << lb << ":\n";
  for(int i=0; i<lb;) {
    for(int j=0; j<21; i++, j++)
      cout << bobOutputs[i];
    cout << endl;
    for(int j=0; j<21; i++, j++)
      cout << bobOutputs[i];
    cout << endl;
    for(int j=0; j<16; i++, j++)
      cout << bobOutputs[i];
    cout << endl << endl;
  }
  cout << endl << endl << endl;
  */

  // ==================== Call Circuit ====================
  int* circuitOperations = new int[MAX_L];
  memset(circuitOperations, 0xff, MAX_L * sizeof(int));
  int (*circuitOperands)[2] = new int[MAX_L][2];
  memset(circuitOperands[0], 0xff, MAX_L * 2 * sizeof(int));
  int (*circuitOutputs)[16] = new int[n][16];
  memset(circuitOutputs[0], 0xff, n * 16 * sizeof(int));
  int l;
  timed("Circuit", [&]() {
    l = circuit(la, lb, circuitOperations, circuitOperands, circuitOutputs);
  });
  cerr << "l = " << l << endl;
  ensure(0 <= l && l <= MAX_L);

  bool* values = new bool[l];
  memcpy(values, aliceOutputs, la * sizeof(bool));
  memcpy(values + la, bobOutputs, lb * sizeof(bool));
  for (int i = la + lb; i < l; i++) {
    int op = circuitOperations[i];
    if (op < 0 || op > 15)
      fatal("Invalid operation: " + to_string(op) + " at Circuit index " + to_string(i));
    int a = circuitOperands[i][0];
    int b = circuitOperands[i][1];
    if (a < 0 || a >= i)
      fatal("Invalid operand 0: " + to_string(a) + " at Circuit index " + to_string(i));
    if (b < 0 || b >= i)
      fatal("Invalid operand 1: " + to_string(b) + " at Circuit index " + to_string(i));
    bool x0 = values[a];
    bool x1 = values[b];
    values[i] = op >> (x0 + 2 * x1) & 1;
  }

  // =============== Debug 2 ===============
  /*
  for(int i=0; i<2048; i++) {
    // if(i == n) cout << "==========" << endl << endl;
    for(int j=0; j<2; j++) {
      for(int k=0; k<21; k++)
        cout << values[deb[i*3+j]+k];
      cout << endl;
    }
    for(int k=0; k<16; k++)
      cout << values[deb[i*3+2]+k];
    cout << endl <<endl;
  }
  */
  /*
  for(auto &i : deb) cout << values[i];
  cout << endl;
  */
  /*
  for(auto &i : deb) {
    for(int j=0; j<16; j++) cout << values[i+j] << ' ';
    cout << endl;
  }
  */
  /*
  for(int i=0; i<38; i++) cout << values[deb[0]+i];
  cout << endl;
  */

  for (int i = 0; i < n; i++)
    for (int j = 0; j < 16; j++)
      if (circuitOutputs[i][j] < 0 || circuitOutputs[i][j] >= l)
        fatal("Invalid output: " + to_string(circuitOutputs[i][j]) + " at Circuit index " + to_string(i) + ", " + to_string(j));
  vector<unsigned short> answers;
  for (int i = 0; i < n; i++) {
    unsigned short ans = 0;
    for (int j = 0; j < 16; j++)
      ans |= (unsigned short) values[circuitOutputs[i][j]] << j;
    answers.push_back(ans);
  }

  // ==================== Calculate Reference Answer ====================
  map<string, unsigned short> referenceMap;
  for (int i = 0; i < m; i++)
    referenceMap[messages[i].second] += studentsMap[messages[i].first];
  bool ok = true;
  for (int i = 0; i < n; i++)
    if (answers[i] != referenceMap[students[i].first]) {
      ok = false;
      cerr << "Wrong answer for " << students[i].first << ": expected " << referenceMap[students[i].first] << ", but got " << answers[i] << endl;
    }
  if (ok)
    cerr << "Your answer seems correct" << endl;

  // ==================== Print Answer ====================
  for (int i = 0; i < n; i++)
    cout << answers[i] << '\n';

  return 0;
}
/*
zzzz = 475253
Before Optimization       --> l = 31342812
NAME_LEN form 21 to 20    --> l = 30249946
compare with lesser gates --> l = 29799386
swap with lesser gates    --> l = 19531756

5 10

d 7
e 6
b 2
a 4
c 7

c b
c e
c c
a d
c e
b e
d b
b a
d c
c d

e d c b a
d e b a c

===

0 1 2
3 4
110 10 0 1 00 0 1 110

---

1 0 4 2 3
100 10 0 1 00 0 1 010

d d c c c c c b b a
c b e e d c b e a d

b c d c d a c b c c
a b b c c d d e e e


Init X    Y
d 7  a 4  a 4
e 6  b 2  b 2
b 2  c 7  c 7
a 4  d 7  d 7
c 7  e 6  e 6

c b  d c  c e
c e  d b  c e
c c  c e  b e
a d  c e  c d
c e  c d  a d
b e  c c  d c
d b  c b  c c
b a  b e  d b
d c  b a  c b
c d  a d  b a

b c d c d a c b c c e d c b a
a b b c c d d e e e e d c b a
0 0 0 0 0 0 0 0 0 0 1 1 1 1 1

b a c d b c d c a c d b c c e
a a b b b c c c d d d e e e e
0 1 0 0 1 0 0 1 0 0 1 0 0 0 1
2 0 7 7 0 7 7 0 4 7 0 2 7 7 0



a b c d e X d d | c c c c c b b a
a b c d e X c b | e e d c b e a d
4 2 7 7 6 X 0 0 | 0 0 0 0 0 0 0 0
0 2 5 1114151312  9 108 7 6 4 3 1

a a b b b c c c | c c c d d d e x
a d b a e c b c | d e e d b c e x
4 0 2 0 0 7 0 0 | 0 0 0 7 0 0 6 x

0 2 5 11 14 15 13 12 | 10 9 8 7 6 4 3 1

00011111 0011 01 0 1 10 1 0 0000 00 0 0 11 0 0

3 5
alic 10000
bob 20000
circ 30000
alic circ
bob circ
bob alic
circ circ
circ circ

*/