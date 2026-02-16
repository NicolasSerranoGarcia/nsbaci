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

### Drawing (Canvas Graphics)

The drawing system uses an SDL-like approach: set the color first, then draw shapes.
Shapes are separated into `draw*` (outline only) and `fill*` (filled shapes).

#### Setting Colors
```cpp
// RGB format (0-255 for each component)
setColor(255, 0, 0);              // Red using RGB values
setColor(0, 255, 0, 128);         // Semi-transparent green (with alpha)

// Predefined color macros
setColor(RED);                    // Use color macro
setColor(GREEN);
setColor(BLUE);
setColor(WHITE);
setColor(BLACK);
setColor(YELLOW);
setColor(CYAN);
setColor(MAGENTA);
setColor(ORANGE);
setColor(PINK);
setColor(PURPLE);
setColor(GRAY);
```

#### Drawing Shapes (Outlines)
```cpp
// Circle: x, y, radius
drawCircle(100, 100, 50);         // Circle outline

// Rectangle: x, y, width, height
drawRect(10, 10, 100, 50);        // Rectangle outline

// Triangle: x1, y1, x2, y2, x3, y3
drawTriangle(100, 50, 50, 150, 150, 150);  // Triangle outline

// Line: x1, y1, x2, y2
drawLine(0, 0, 100, 100);

// Ellipse: x, y, radiusX, radiusY
drawEllipse(200, 200, 60, 30);    // Ellipse outline

// Single pixel
drawPixel(50, 50);

// Text: x, y, "string" [, fontSize]
drawText(10, 20, "Hello World!");
drawText(10, 50, "Large text", 24);
```

#### Filling Shapes (Solid)
```cpp
// Filled circle: x, y, radius
fillCircle(200, 200, 30);         // Solid circle

// Filled rectangle: x, y, width, height
fillRect(150, 10, 80, 40);        // Solid rectangle

// Filled triangle: x1, y1, x2, y2, x3, y3
fillTriangle(200, 50, 150, 150, 250, 150);  // Solid triangle

// Filled ellipse: x, y, radiusX, radiusY
fillEllipse(300, 200, 40, 20);    // Solid ellipse
```

#### Using Points with {x, y} Syntax
Shapes can also receive coordinates as Points using `{x, y}` syntax:
```cpp
// Circle with Point
drawCircle({100, 100}, 50);       // Same as drawCircle(100, 100, 50)
fillCircle({200, 200}, 30);

// Rectangle with Point
drawRect({10, 10}, 100, 50);      // x, y as Point, then width, height
fillRect({150, 10}, 80, 40);

// Triangle with three Points
drawTriangle({100, 50}, {50, 150}, {150, 150});
fillTriangle({200, 50}, {150, 150}, {250, 150});

// Line with two Points
drawLine({0, 0}, {100, 100});

// Ellipse with Point
drawEllipse({200, 200}, 60, 30);
fillEllipse({300, 200}, 40, 20);

// Pixel with Point
drawPixel({50, 50});

// Text with Point
drawText({10, 20}, "Hello World!");
drawText({10, 50}, "Large text", 24);
```

#### Position Macros
```cpp
// Use predefined positions (based on 800x600 canvas)
drawCircle(CENTER, 50);           // Circle at center
drawCircle(TOP_LEFT, 30);
drawCircle(TOP_CENTER, 30);
drawCircle(TOP_RIGHT, 30);
drawCircle(CENTER_LEFT, 30);
drawCircle(CENTER_RIGHT, 30);
drawCircle(BOTTOM_LEFT, 30);
drawCircle(BOTTOM_CENTER, 30);
drawCircle(BOTTOM_RIGHT, 30);
```

#### Canvas Operations
```cpp
clearCanvas();                    // Clear with default (white) background
clearCanvas(0, 0, 0);            // Clear with black
clearCanvas(BLUE);               // Clear with color macro

setLineWidth(3);                  // Set line thickness for subsequent draws

refreshCanvas();                  // Force canvas refresh
```

#### Complete Drawing Example
```cpp
// Draw a simple house
clearCanvas(WHITE);

// House body
setColor(139, 69, 19);            // Brown color (RGB)
fillRect(100, 200, 200, 150);

// Roof
setColor(RED);
fillTriangle({100, 200}, {200, 100}, {300, 200});

// Door
setColor(YELLOW);
fillRect({175, 280}, 50, 70);

// Windows
setColor(CYAN);
fillRect({120, 230}, 40, 40);
fillRect({240, 230}, 40, 40);

// Sun
setColor(YELLOW);
fillCircle({400, 80}, 40);

// Ground
setColor(GREEN);
fillRect({0, 350}, 800, 250);

// Label
setColor(BLACK);
drawText({150, 380}, "My House", 20);

refreshCanvas();
return;
```

## Concurrency Notes

- The scheduler uses **random thread selection** to simulate non-deterministic execution
- Running the same concurrent program multiple times may produce different output orderings
- This helps demonstrate race conditions and the importance of proper synchronization
- Use semaphores to ensure correct behavior in concurrent programs
