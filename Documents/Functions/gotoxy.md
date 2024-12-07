# Function name : gotoxy
## Function prototype

```cpp
# Function name : createPrefab

## Function prototype

```cpp
void gotoxy(int x, int y);
```

## Implementation

```cpp
void Silver::gotoxy(int x, int y) {
  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;
  cout << "\033[" << y + 1 << ";" << x + 1 << "H" << flush;
}
```

## Description
This function moves the cursor to a certain position in a console.

## Example Usage
```cpp
for(int i=0; i<10; i++) {
  Silver::gotoxy(0,i);
  printf("#");
  Silver::gotoxy(9-i,0);
  printf("#");
  Silver::gotoxy(9,9-i);
  printf("#");
  Silver::gotoxy(i,9);
  printf("#");

  Silver::wait(100);
} // I like this
```
