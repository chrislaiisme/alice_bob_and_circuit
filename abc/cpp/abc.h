// #include "abc.h"
// #define DEBUG
#pragma GCC optimize("Ofast")
#include<iostream>
#include<algorithm>
#include<string>
#include<vector>
#include<set>
#include<map>
#include<math.h>
#include<fstream>
using namespace std;

// you may find the definitions useful
const int OP_ZERO    = 0;  // f(OP_ZERO,    x0, x1) = 0
const int OP_NOR     = 1;  // f(OP_NOR,     x0, x1) = !(x0 || x1)
const int OP_GREATER = 2;  // f(OP_GREATER, x0, x1) = (x0 > x1)
const int OP_NOT_X1  = 3;  // f(OP_NOT_X1,  x0, x1) = !x1
const int OP_LESS    = 4;  // f(OP_LESS,    x0, x1) = (x0 < x1)
const int OP_NOT_X0  = 5;  // f(OP_NOT_X0,  x0, x1) = !x0
const int OP_XOR     = 6;  // f(OP_XOR,     x0, x1) = (x0 ^ x1)
const int OP_NAND    = 7;  // f(OP_NAND,    x0, x1) = !(x0 && x1)
const int OP_AND     = 8;  // f(OP_AND,     x0, x1) = (x0 && x1)
const int OP_EQUAL   = 9;  // f(OP_EQUAL,   x0, x1) = (x0 == x1)
const int OP_X0      = 10; // f(OP_X0,      x0, x1) = x0
const int OP_GEQ     = 11; // f(OP_GEQ,     x0, x1) = (x0 >= x1)
const int OP_X1      = 12; // f(OP_X1,      x0, x1) = x1
const int OP_LEQ     = 13; // f(OP_LEQ,     x0, x1) = (x0 <= x1)
const int OP_OR      = 14; // f(OP_OR,      x0, x1) = (x0 || x1)
const int OP_ONE     = 15; // f(OP_ONE,     x0, x1) = 1

const int NUM_LEN = 16, NAME_LEN = 20, OBJ_LEN = 56;
vector<int> deb;
#define FOR_NUM(i) for(int i=0; i<NUM_LEN; ++i)
#define FOR_NAME(i) for(int i=0; i<NAME_LEN; ++i)
#define all(vec) vec.begin(), vec.end()
#define pii pair<int, int>
#define F first
#define S second
#define mkp make_pair

fstream nothing;
#ifdef DEBUG
#define debug cout
#else
#define debug nothing
#endif

struct stc {
  int X, Y, W;
  stc(int x=0, int y=0, int w=0): X(x), Y(y), W(w) {}
};

string Str = "";
void Perm(vector<int> vec) {
  int n = vec.size();
  if(n <= 1) return;
  bool odd=0;
  if(n&1) {
    vec.push_back(n);
    ++n;
    odd=1;
  }
  int len = n/2;
  vector<int> pos(n);
  vector<bool> vst(len, 0), change(len, 0);

  auto Swap = [&](int p) {
    change[p] = !change[p];
    swap(vec[p], vec[p+len]);
    pos[vec[p]] = p;
    pos[vec[p+len]] = p+len;
  };

  for(int i=0; i<n; i++) pos[vec[i]] = i;
  for(int i=0; i<len; i++) {
    if(vst[vec[i]%len]) continue;
    int num = vec[i];
    while(!vst[num%len]) {
      vst[num%len] = 1;
      int a = num, b = (num<len ? num+len : num-len);
      if((pos[a]<len && pos[b]<len) || (pos[a]>=len && pos[b]>=len)) Swap(pos[b]%len);
      num = vec[(pos[b]<len ? pos[n]+len : pos[b]-len)];
    }
  }
  if(change[len-1] == 1) {
    for(int i=0; i<len; i++) Swap(i);
  }
  for(int i=0; i<len; i++) Str += (char)('0'+change[i]);
  for(int i=0; i<len; i++) change[vec[i]%len] = (vec[i]>=len);
  for(auto &i : vec) i %= len;
  Perm(vector<int>(vec.begin(), vec.begin()+len));
  Perm(vector<int>(vec.begin()+len, vec.end() - odd));
  for(int i=0; i<len; i++) Str += (char)('0'+change[i]);
}

// Alice
int // returns la
alice(
  /*  in */ const int n,
  /*  in */ const char names[][5],
  /*  in */ const unsigned short numbers[],
  /* out */ bool outputs_alice[]
) {

  // =============== Convert to Object ===============
  vector<stc> vec(n);
  for(int i=0; i<n; ++i) {
    int name=0, num=numbers[i];
    string str = names[i];
    for(auto &j : str) name = name*26+(j-'a');
    for(int i=0, tmp=26; i<str.length()-1; i++, tmp*=26) name += tmp;
    vec[i] = stc(name, name, num);
  }
  // =============== Get Pemutation of Ydown -> Init ===============
  vector<int> sorted(n), perm(n);
  for(int i=0; i<n; ++i) sorted[i] = vec[i].Y;
  sort(all(sorted), greater<int>());
  map<int, int> mp;
  for(int i=0; i<n; ++i) mp[vec[i].Y] = i;
  for(int i=0; i<n; ++i) perm[i] = mp[sorted[i]];
  string Ydown_Init = "";
  Str.clear();
  Perm(perm);
  Ydown_Init = Str;
  // =============== Sort vec and Pass Data to Circuit (Xup + Ydown_Init) ===============
  sort(vec.begin(), vec.end(), [&](stc a, stc b) {return a.X < b.X;});
  int cur=0;
  for(int i=0; i<n; ++i) {
    outputs_alice[cur++] = 0;
    for(int j=0; j<NAME_LEN-1; ++j)
      outputs_alice[cur++] = (vec[i].X >> j) & 1;
    outputs_alice[cur++] = 1;
    for(int j=0; j<NAME_LEN-1; ++j)
      outputs_alice[cur++] = (vec[i].Y >> j) & 1;
    FOR_NUM(j)
      outputs_alice[cur++] = (vec[i].W >> j) & 1;
  }
  for(int i=0; i<(int)Ydown_Init.length(); ++i) outputs_alice[cur++] = Ydown_Init[i] - '0';
  while(cur != 69*n) outputs_alice[cur++] = 0;
  return cur;
}


// Bob
int // returns lb
bob(
  /*  in */ const int m,
  /*  in */ const char senders[][5],
  /*  in */ const char recipients[][5],
  /* out */ bool outputs_bob[]
) {

  // =============== Convert to Object ===============
  vector<pair<pii, int> > sorted(m);
  for(int i=0; i<m; ++i) {
    int s=0, r=0;
    string S = senders[i], R = recipients[i];
    for(auto &j : S) s = s*26+(j-'a');
    for(int i=0, tmp=26; i<S.length()-1; i++, tmp*=26) s += tmp;
    for(auto &j : R) r = r*26+(j-'a');
    for(int i=0, tmp=26; i<R.length()-1; i++, tmp*=26) r += tmp;
    sorted[i].F = make_pair(s, r);
  }

  // =============== Get Pemutation of Xdown -> Yup ===============
  vector<int> perm(m);
  sort(all(sorted), [&](pair<pii, int> a, pair<pii, int> b) {if(a.F.S != b.F.S) return a.F.S < b.F.S; return a.F.F < b.F.F;});
  for(int i=0; i<m; ++i) sorted[i].S = i;
  sort(all(sorted), [&](pair<pii, int> a, pair<pii, int> b) {if(a.F.F != b.F.F) return a.F.F > b.F.F; return a.F.S > b.F.S;});
  for(int i=0; i<m; ++i) perm[i] = sorted[i].S;
  string Xdown_Yup = "";
  Str.clear();
  Perm(perm);
  Xdown_Yup = Str;

  // =============== Pass Data to Circuit (Xdown + Xdown_Yup) ===============
  int cur=0;
  for(int i=0; i<m; ++i) {
    outputs_bob[cur++] = 1;
    for(int j=0; j<NAME_LEN-1; ++j)
      outputs_bob[cur++] = (sorted[i].F.F >> j) & 1;
    outputs_bob[cur++] = 0;
    for(int j=0; j<NAME_LEN-1; ++j)
      outputs_bob[cur++] = (sorted[i].F.S >> j) & 1;
    FOR_NUM(j)
      outputs_bob[cur++] = 0;
  }
  for(int i=0; i<(int)Xdown_Yup.length(); ++i) outputs_bob[cur++] = Xdown_Yup[i] - '0';
  while(cur != 69*m) outputs_bob[cur++] = 0;
  return cur;
}

// Circuit
int // returns l
circuit(
  /*  in */ const int la,
  /*  in */ const int lb,
  /* out */ int operations[],
  /* out */ int operands[][2],
  /* out */ int outputs_circuit[][16]
) {
  int cur = la + lb;
  /**/ debug << cur << endl;
  // =============== Initialization ===============
  auto op = [&](int o, int a, int b) {
    operations[cur] = o;
    operands[cur][0] = a, operands[cur][1] = b;
    return cur ++ ;
  };
  int zero = cur, ZERO = cur;
  FOR_NUM(i) op(OP_ZERO, 0, 0);
  int one = cur, INF = cur, INF_NAME = cur;
  FOR_NAME(i) op(OP_ONE, 1, 1);

  auto ADD = [&](int a, int b) {
    vector<int> carry = {zero}, digit;
    FOR_NUM(i) {
      int d = op(OP_XOR, a+i, b+i);
      int c1 = op(OP_AND, a+i, b+i);
      int c2 = op(OP_AND, d, carry[i]);
      if(i != NUM_LEN-1) carry.push_back(op(OP_OR, c1, c2));
      digit.push_back(d);
    }
    int ret = cur;
    FOR_NUM(i) op(OP_XOR, digit[i], carry[i]);
    return ret;
  };

  // ================ Solve ===============
  int n = la/69, m = lb/69;
  int SIZE = 1;
  while(SIZE < n+m) SIZE *= 2;
  vector<vector<int> > arr(3, vector<int>(SIZE)); // 0 = X, 1 = Y, 2 = W;
  for(int i=0; i<n; i++) {
    arr[0][i] = OBJ_LEN*i;
    arr[1][i] = arr[0][i] + NAME_LEN;
    arr[2][i] = arr[1][i] + NAME_LEN;
  }
  for(int i=0; i<m; i++) {
    arr[0][SIZE-m+i] = la+OBJ_LEN*i;
    arr[1][SIZE-m+i] = arr[0][SIZE-m+i] + NAME_LEN;
    arr[2][SIZE-m+i] = arr[1][SIZE-m+i] + NAME_LEN;
  }
  for(int i=n; i<SIZE-m; i++) {
    arr[0][i] = INF_NAME;
    arr[1][i] = INF_NAME;
    arr[2][i] = INF;
  }

  // ===== Add Numbers to Letters =====
  vector<int> signals;

  auto swap = [&](int a, int b, int s) {
    int ns = op(OP_NOT_X0, s, s);
    // vector<int> veca[3], vecb[3];
    for(int j=0; j<3; j++) {
      int len = (j == 2 ? NUM_LEN : NAME_LEN);
      vector<pii> vec;
      for(int i=0; i<len; i++) {
        int p = arr[j][a]+i, q = arr[j][b]+i;
        vec.push_back(mkp(op(OP_AND, p, ns), op(OP_AND, q, s)));
      }
      int aa = cur; for(auto &i : vec) op(OP_OR, i.F, i.S);
      vec.clear();
      for(int i=0; i<len; i++) {
        int p = arr[j][a]+i, q = arr[j][b]+i, num = aa+i;
        vec.push_back(mkp(op(OP_XOR, num, p), q));
      }
      int bb = cur; for(auto &i : vec) op(OP_XOR, i.F, i.S);
      arr[j][a] = aa;
      arr[j][b] = bb;
    }
    /*
    int ns = op(OP_NOT_X0, s, 0);
    veca[j].push_back(op(OP_OR, op(OP_AND, p, ns), op(OP_AND, q, s)));
    int num = veca[j].back();
    vecb[j].push_back(op(OP_XOR, op(OP_XOR, num, p), q));
    */
    /*
    for(int i=0; i<3; i++) {
      int aa = cur; for(auto &j : veca[i]) op(OP_OR, j, 0);
      int bb = cur; for(auto &j : vecb[i]) op(OP_OR, j, 0);
      arr[i][a] = aa;
      arr[i][b] = bb;
    }
    */
  };

  auto cmp = [&](int a, int b, int k) {
    int P = arr[k][a], Q = arr[k][b];
    if(Q == INF) {
      signals.push_back(zero);
      return;
    }
    if(P == INF) {
      signals.push_back(one);
      swap(a, b, one);
      return;
    }
    int s = zero;
    FOR_NAME(i) {
      int p = P+i, q = Q+i;
      int eq = op(OP_EQUAL, p, q), gr = op(OP_GREATER, p, q);
      s = op(OP_AND, s, eq);
      s = op(OP_OR, s, gr);
      /*
      s\(eq gr) 00 10 01
      0          0  0  1
      1          0  1  1

          0 1   --> &eq |gr
      0   0 0
      1   0 1
      */
    }
    signals.push_back(s);
    swap(a, b, s);
  };

  auto merge = [&](auto merge1, int l, int r, int k) {
    if(l >= r) return;
    int m = (l+r)>>1, len = m-l+1;
    for(int i=l; i<=m; i++) cmp(i, i+len, k);
    merge1(merge1, l, m, k); merge1(merge1, m+1, r, k);
  };
  merge(merge, 0, SIZE-1, 0);
  int num = ZERO;
  for(int i=0; i<n+m; i++) {
    int tmp = cur;
    FOR_NUM(j) op(OP_AND, num+j, arr[0][i]);
    int pos = cur;
    FOR_NUM(j) op(OP_OR, tmp+j, arr[2][i]+j);
    arr[2][i] = pos;
    num = arr[2][i];
  }

  /**/ debug << "Add Numbers to Letters Complete" << endl;
  /**/ debug << cur << endl;

  // ===== Unmerge =====
  // for(auto &i : signals) debug << i << ' ';
  // debug << endl;
  reverse(all(signals));
  int p=0;
  auto unmerge = [&](auto unmerge1, int l, int r) {
    if(l >= r) return;
    int m = (l+r)>>1, len = m-l+1;
    unmerge1(unmerge1, m+1, r); unmerge1(unmerge1, l, m);
    for(int i=m; i>=l; i--) {
      // debug << i << ' ' << i+len << ' ' << signals[p] << " ???" << endl;
      if(i+len <= r) swap(i, i+len, signals[p]);
      p++;
    }
  };
  unmerge(unmerge, 0, SIZE-1);
  /*
  for(int i=0; i<SIZE; i++) {
    deb.push_back(arr[0][i]);
    deb.push_back(arr[1][i]);
    deb.push_back(arr[2][i]);
  }
  */
  /**/ debug << "Unmerge Complete" << endl;
  /**/ debug << cur << endl;

  // ===== Change Permutation =====
  int perm = la + OBJ_LEN*m;
  p=0;
  auto applyperm = [&](auto applyperm1, int l, int r) {
    if(l >= r) return;
    int num = r-l+1, len = (num+1)/2;

    // ===== Change 1 =====
    for(int i=l; i<l+len; i++) {
      if(i+len <= r)
        swap(i, i+len, perm+p);
      p++;
    }

    // ===== Recursion =====
    applyperm1(applyperm1, l, l+len-1);
    applyperm1(applyperm1, l+len, r);

    // ===== Change 2 =====
    for(int i=l; i<l+len; i++) {
      if(i+len <= r)
        swap(i, i+len, perm+p);
      p++;
    }
  };
  applyperm(applyperm, SIZE-m, SIZE-1);
  
  auto arr2 = arr;
  
  for(int i=0; i<m; i++)
    for(int j=0; j<3; j++)
      arr[j][i] = arr2[j][SIZE-m+i];
  for(int i=0; i<SIZE-n-m; i++)
    for(int j=0; j<3; j++)
      arr[j][m+i] = arr2[j][n+i];
  for(int i=0; i<n; i++)
    for(int j=0; j<3; j++)
      arr[j][SIZE-n+i] = arr2[j][n-1-i];

  /**/ debug << "Change Permutation Complete" << endl;
  /**/ debug << cur << endl;

  // ===== Add Number to Person =====
  signals.clear();
  merge(merge, 0, SIZE-1, 1);
  num = ZERO;
  for(int i=0; i<n+m; i++) {
    int t = arr[1][i], nt = op(OP_NOT_X0, t, 0);
    int reset = cur;
    FOR_NUM(j) op(OP_AND, arr[2][i]+j, nt);
    arr[2][i] = reset;
    int num2 = cur;
    FOR_NUM(j) op(OP_AND, num+j, t);
    arr[2][i] = ADD(arr[2][i], num2);
    int tmp = cur;
    FOR_NUM(j) op(OP_AND, arr[2][i]+j, nt);
    num = ADD(num, tmp);
    int num3 = cur;
    FOR_NUM(j) op(OP_AND, num+j, nt);
    num = num3;
  }
  /*
  for(int i=0; i<SIZE; i++) {
    deb.push_back(arr[0][i]);
    deb.push_back(arr[1][i]);
    deb.push_back(arr[2][i]);
  }
  */
  // deb = signals;
  /**/ debug << "Add Number to Person Complete" << endl;
  /**/ debug << cur << endl;

  // ===== Unmerge =====
  reverse(all(signals));
  p=0;
  unmerge(unmerge, 0, SIZE-1);

  /**/ debug << "Unmerge Complete" << endl;
  /**/ debug << cur << endl;

  // ===== Change Permutation =====
  perm = OBJ_LEN*n;
  p=0;
  applyperm(applyperm, SIZE-n, SIZE-1);

  /**/ debug << "Change Permutation Complete" << endl;
  /**/ debug << cur << endl;

  // ===== Output Answer =====
  for(int i=0; i<n; i++) {
    int ans = arr[2][SIZE-n+i];
    FOR_NUM(j) outputs_circuit[i][j] = ans + j;
  }

  /**/ debug << "Solve Complete" << endl;
  /**/ debug << cur << endl;

  return cur;
}
/*
  for(int i=0; i<n+m; i++) {
    deb.push_back(arr[0][i]);
    deb.push_back(arr[1][i]);
    deb.push_back(arr[2][i]);
  }
  
*/