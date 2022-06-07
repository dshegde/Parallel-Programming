//----------------------------------------------------------------------------- 
// Program code for CS 415/515 Parallel Programming, Portland State University.
//----------------------------------------------------------------------------- 

// The sum program (Java version).
//
//
class Sum extends java.lang.Thread {
  static final int N = 1000; 
  static final int P = 10; 
  static int sum = 0; 

  int low, high, psum;

  Sum(int tid) {
    low = (N/P) * tid; 	 // a simplistic partition scheme
    high = low + (N/P);
    psum = 0;
  }

  int compute(int i) { return i*i; }

  public void run() {    // this method is required
    for (int i = low; i < high; i++)
      psum += compute(i);
    sum += psum;
  }

  public static void main(String[] args) {
    try {
      Sum[] threads = new Sum[P];
      for (int k = 0; k < P; k++)   // create threads
        threads[k] = new Sum(k);
      for (int k = 0; k < P; k++)   // start threads
        threads[k].start();
      for (int k = 0; k < P; k++)   // join threads
	threads[k].join();
      System.out.println("The sum is " + sum + " (should be 332833500)");
      if (sum != 332833500)
        System.out.println("*** Found an incorrect answer, " + sum + "! ***");
    } catch (Exception e) {
      System.err.println(e.getMessage());
    }
  }
}

