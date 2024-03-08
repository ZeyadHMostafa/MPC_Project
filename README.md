# Microprocessors C Project
An overly simplified toy model of a Hospital registration system
### Student Information
- **Name:** Zeyad Hesham Mostafa
- **ID:** 2020/11163

### Project Description
A terminal interface that allows login to access the hospital's data, allowing viewing and editing of some of it.

### Functionality Overview
Available operations include:
- Logging in as a staff member or a guest
- Registering a new user (Admin users only)
- Checking empty rooms (Allowed for Guests)
- Updating Patient Status
- Transferring Patients to different rooms
- Discharging Patients

## Methodology
### Interface Goals

- Show menu title during repeated requests such as login
- Separate different operations to avoid confusion
- Use patient ID as the main way of referencing patients
- Display necessary data when searching through arrays
- Show decision confirmation messages when necessary
- Allow change of decision for common mistakes
- Show confirmation messages for operation success
- Have basic input sanitization (not advanced but not none)

### Code Goals

- Avoid long stretches of code (use functions for separation)
- Have code sections representing related functions
- Create generic functions for scalability
- Balance scalability and performance as required
- Use Constants and Enums to reduce "hard coded" values
- Balance code readability with line count
- Keep The main function as small as possible
- Use library functions when appropriate

### Development Steps

1. Write a basic version of a desired operation
2. Write a test in main to confirm functionality
3. Include helper libraries to simplify code
4. Simplify some sections into sub-functions
5. Redefine necessary constants outside the function
6. Redefine global variables outside of the function
7. Add extra functionality to the operation
8. Reorder functions to fit into clean sections
9. Refactor code for efficiency when possible
10. Comment and refactor code for readability

### Notes
- Remove the user "a" to text root funcitonality.
- With this many operations, the interface should be refactored.
