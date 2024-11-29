# Function name : all

## Function prototype

```cpp
void Silver::setConsoleTitle(const string title);
```

## Implementation

```cpp
void Silver::setConsoleTitle(const string & title) {
  cout << "\033]0;" << title << "\007";
}
```

## Description
This function sets the title of the console. 

## Example Usage
```cpp
silver.setConsoleTitle("Hello World");
```
