# Railway Reservation and Waiting List Management System

A menu-driven Data Structures and Algorithms (DSA) mini project written in C. It simulates basic railway ticket booking, cancellation, reservation lookup, and waiting-list management.

## Features

- Display available trains and seat counts
- Show a seat map for a selected train
- Book a ticket and generate a PNR for confirmed reservations
- Add passengers to a FIFO waiting list when seats are unavailable
- Search confirmed reservations by PNR
- Cancel a confirmed reservation
- Promote the first waiting passenger after a cancellation
- Display confirmed reservations and waiting lists
- Sort confirmed reservations by PNR
- Display train statistics and the PNR BST traversal

## Data Structures and Algorithms

| Structure / technique | Use |
| --- | --- |
| Array | Stores predefined train records and seat counts |
| Linked list | Stores confirmed reservations |
| FIFO queue | Stores waiting passengers in arrival order |
| Binary Search Tree (BST) | Indexes confirmed reservations by PNR |
| Sorting | Displays confirmed reservations in PNR order |
| Dynamic memory allocation | Creates and releases booking, queue, and BST nodes |

> Note: A Trie is not implemented in this version. Trie-based train/station autocomplete can be added as a future enhancement if required.

## Requirements

A C compiler, such as GCC or Clang.

## Compile and Run

### macOS / Linux

```bash
gcc railway_reservation_system.c -o railway_reservation_system
./railway_reservation_system
```

### Windows (MinGW GCC)

```bash
gcc railway_reservation_system.c -o railway_reservation_system.exe
railway_reservation_system.exe
```

## How to Use

1. Run the program.
2. Select an option from the main menu.
3. Display trains and choose a train number.
4. Enter passenger details to book a ticket.
5. Use the generated PNR to search or cancel a confirmed reservation.
6. If the train is full, the passenger is added to that train's waiting list.
7. Select Exit to close the program.

## Project Scope

This is an educational console-based simulation. It uses in-memory data, so bookings are not saved after the program exits. It is not connected to a real railway reservation service.

## Team

- Member 1: [Add name and roll number]
- Member 2: [Add name and roll number]

## Future Enhancements

- Persistent storage using file handling or a database
- Trie-based train-name or station-name autocomplete
- Graph-based route search
- Date-wise schedules and additional travel classes
- Improved reporting and user authentication
