#include <bits/stdc++.h>
using namespace std;

inline void nocpufence(volatile int &x, volatile int &y, volatile int &r) {
  x = 1;
  asm volatile("" ::: "memory"); // barrier in compiler level
  r = y;
}

inline void mfence(volatile int &x, volatile int &y, volatile int &r) {
  x = 1;
  asm volatile("mfence" ::: "memory");
  r = y;
}

inline void lockadd(volatile int &x, volatile int &y, volatile int &r) {
  x = 1;
  asm volatile("lock; addl $0,0(%%rsp)" ::: "memory", "cc");
  r = y;
}

inline void xchg(volatile int &x, volatile int &y, volatile int &r) {
  int tmp = 1;
  asm volatile("xchgl %0, %1" : "+r"(tmp), "+m"(x)::"memory", "cc");
  r = y;
}

volatile int g_cnt = 0;
void threadfun(volatile int &loop_cnt, volatile int &x, volatile int &y,
               volatile int &r) {
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  while (true) {
    while (g_cnt == loop_cnt)
      ;

    asm volatile("" ::: "memory");

    nocpufence(x, y, r);
    //mfence(x, y, r);
    //lockadd(x, y, r);
    //xchg(x, y, r);

    asm volatile("" ::: "memory");
    loop_cnt++;
  }
}

int main() {
  alignas(64) volatile int cnt1 = 0;
  alignas(64) volatile int cnt2 = 0;
  alignas(64) volatile int x = 0;
  alignas(64) volatile int y = 0;
  alignas(64) volatile int r1 = 0;
  alignas(64) volatile int r2 = 0;
  thread thr1(threadfun, ref(cnt1), ref(x), ref(y), ref(r1));
  thread thr2(threadfun, ref(cnt2), ref(y), ref(x), ref(r2));

  {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(1, &cpuset);
    int rc = pthread_setaffinity_np(thr1.native_handle(), sizeof(cpu_set_t),
                                    &cpuset);
    if (rc != 0) {
      std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
    }
  }
  {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(2, &cpuset);
    int rc = pthread_setaffinity_np(thr2.native_handle(), sizeof(cpu_set_t),
                                    &cpuset);
    if (rc != 0) {
      std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
    }
  }

  int detected = 0;
  while (true) {
    x = y = 0;
    asm volatile("" ::: "memory");
    g_cnt++;
    while (cnt1 != g_cnt || cnt2 != g_cnt)
      ;

    asm volatile("" ::: "memory");
    if (r1 == 0 && r2 == 0) {
      detected++;
      cout << "bad, g_cnt: " << g_cnt << " detected: " << detected << endl;
    }
  }
  return 0;
}
