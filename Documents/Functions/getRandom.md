# Function name : getRandom

## Function prototype

```cpp
int getRandom(int min, int max);
```

## Implementation

```cpp
int Silver::getRandom(int min, int max) {
    static mt19937 rng(random_device{}() ^ 
                       chrono::system_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}
```


## Description
This function picks a random integer. The minimum value is `min` and the maximum value is `max`.
