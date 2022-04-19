# Linear Snake
A simple tracking algorithm where a variable number of control points adjust themselves, appear and disappear as required to track the shape deformation of an object across a series of video images.

# Getting Started
Requires opencv.
1.	Use tracking demo to generate file with control point history info

# Build and Test
mkdir compile
cd compile
cmake ..
make

# History Sample Usage
run ./SimpleTracker/simple_tracker to perform tracking.  It will generate a file with the history of every control point through the indicated video.

## Other tests
./SimpleTracker/play_video reproduces a video and performs tracking
./SimpleTracker/replay_history is a useful test to check that the history was saved as it had to