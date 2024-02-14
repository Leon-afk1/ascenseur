# Elevator Simulation School Project 

## Overview

This project aims to simulate the operation of elevators in a multi-floor building. It includes features such as user generation, elevator movement, and user management.

## Features

- **Multi-elevator Simulation**: The project simulates the operation of two elevators within a building with multiple floors.
- **User Generation**: Random users are generated with varying floor call and destination floor preferences.
- **Elevator Logic**: The elevators follow a logic for picking up and serving users based on their floor call and destination.

## Progress

- **Completed Features**: Basic elevator movement, user generation, and management have been implemented.
- **Issues Encountered**: The project has faced segmentation faults, possibly due to memory issues and thread synchronization problems.
- **Abandoned Points**: Due to a tight deadline, certain features or improvements might not have been fully implemented or optimized.
  - **Pick-Up Along the Way**: Initially, there was a goal to allow elevators to pick up passengers along their route when the elevator is moving in the same direction as the user. Several functions were designed for this purpose, but due to unresolved issues, they were not fully implemented.
  - **Pick-Up Several Users** : Initially, it was another goal to allow elevators to picks severals passengers at the same times, but because of the first point, we coulnt't in time.


## Known Issues

- **Segmentation Fault**: There is a persistent issue with segmentation faults that needs to be addressed.
- **Thread Synchronization**: The project might have issues related to thread synchronization, leading to unexpected behavior.

## Future Improvements

- **Memory Management**: Refactor and improve memory management to avoid segmentation faults.
- **Thread Safety**: Enhance thread safety to prevent synchronization issues.
- **Additional Features**: Implement additional features, such as user input for elevator control and a more realistic simulation environment.

## Deadline Constraints

The project faced time constraints, leading to compromises on certain aspects. These compromises include not fully addressing known issues and leaving out additional features initially planned.

## How to Use

1. Clone the repository.
2. Compile the code using a C compiler (`gcc` recommended).
3. Run the executable.
4. Monitor the simulation in the console.

## Contributors

- Leroy Fernand
- Faravel Félix
- Morales Léon


