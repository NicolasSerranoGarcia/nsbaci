# NSBACI Example Programs

This folder contains example programs demonstrating all the features of the NSBACI language.

## Language Quick Reference

### Data Types
```cpp
int x = 10;           // Integer
bool flag = true;     // Boolean
char c = 'A';         // Character
const int MAX = 100;  // Constant
```

### Arrays
```cpp
int arr[10];          // Declare array of 10 integers
arr[0] = 5;           // Assign to element
int x = arr[0];       // Read element
```

### Control Flow
```cpp
if (cond) { } else { }           // Conditional
while (cond) { }                  // While loop
do { } while (cond);              // Do-while loop
for (int i = 0; i < n; i++) { }  // For loop
break;                            // Exit loop
continue;                         // Skip to next iteration
```

### Functions
```cpp
int add(int a, int b) {
    return a + b;
}

void greet() {
    cout << "Hello!" << endl;
    return;
}
```

### Input/Output
```cpp
cout << "Hello" << endl;         // Print with newline
cout << "x = " << x << endl;     // Print variable
cin >> x;                         // Read integer into x
```

### Concurrency
```cpp
// Parallel execution
cobegin
{
    // Thread 1 code
}
{
    // Thread 2 code
}
coend

// Semaphores
semaphore mutex = 1;    // Binary semaphore (mutex)
semaphore count = 5;    // Counting semaphore

p(mutex);               // Wait/acquire (P operation)
// Critical section
v(mutex);               // Signal/release (V operation)
```

## Running Examples

1. Open NSBACI application
2. Load an example file (File → Open)
3. Compile the program
4. Run or step through execution
5. Observe output and thread states

## Concurrency Notes

- The scheduler uses **random thread selection** to simulate non-deterministic execution
- Running the same concurrent program multiple times may produce different output orderings
- This helps demonstrate race conditions and the importance of proper synchronization
- Use semaphores to ensure correct behavior in concurrent programs
