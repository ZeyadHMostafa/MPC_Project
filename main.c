#include <stdio.h>
#include <string.h>
#include <stdlib.h>	
#include <ctype.h>

//------------------------------------------------------------------------------------------------------
// Constants


// Define constant values
#define ROOM_COUNT 50
#define MAX_PATIENT_COUNT 50
#define MAX_USER_COUNT 50
#define STRING_MAX_LEN 50

char S_SEPARATOR[] = "-------------------------------------------------------------------------";
char S_CANCELLED[] = "Operation cancelled";

// Define validation constants
char VALID_USERNAME_CHARS[] = "_";
char VALID_USERNAME_NUM_ALLOWED=1;
char VALID_USERNAME_MESSAGE[] = "alphanumeric characters or underscores";
char VALID_USERNAME_MIN_LEN = 5;
char VALID_USERNAME_MAX_LEN = 40;

char VALID_PASSWORD_CHARS[] = "_!@#$\%^&*";
char VALID_PASSWORD_NUM_ALLOWED=1;
char VALID_PASSWORD_MESSAGE[] = "alphanumeric characters or any of [!@#$\%^&*]";
char VALID_PASSWORD_MIN_LEN = 8;
char VALID_PASSWORD_MAX_LEN = 40;

char VALID_PATIENT_CHARS[] = "";
char VALID_PATIENT_NUM_ALLOWED=0;
char VALID_PATIENT_MESSAGE[] = "alphabetic characters";
char VALID_PATIENT_MIN_LEN = 1;
char VALID_PATIENT_MAX_LEN = 20;

// Enums
enum Privs {NOPRV,ADMIN,STAFF,GUEST,};
char PrivsToS[4][20] = {"No Privelage","Admin","Staff","Guest"};

enum RoomStatus {VACANT,FULL};
char RoomStatusToS[2][20] = {"Vacant","Full"};

enum PatientStatus {VISIT,RECOVER,ILL,SEVERE,DISMISSED};
char PatientStatusToS[5][20] = {"Visit","Recover","Ill","Severe","Dismissed"};

// Define User struct and default values
struct User {
	char name[STRING_MAX_LEN];
	char password[STRING_MAX_LEN]; //passwords shouldn't be saved in plain text, add some encoding maybe?
	enum Privs privilege;
} 
NOT_A_USER = {.privilege = NOPRV, .name = "", .password = ""}, // for failed login attempts
GUEST_USER = {.privilege = GUEST, .name = "", .password = ""}, // for guest allowed actions
ROOT_ADMIN = {.privilege = ADMIN, .name = "", .password = ""}; // for when there is no admin present (initial setup)

// Define Room struct
struct Room {
	unsigned char id;
	unsigned int patient_id;
	enum RoomStatus status;
};

// Define Patient struct
struct Patient {
	char name[STRING_MAX_LEN];
	unsigned int id;
	enum PatientStatus status;
};


//------------------------------------------------------------------------------------------------------
// Initial Data


// Define available users
struct User users[MAX_USER_COUNT] = {
	{
		.privilege = STAFF,
		.name = "John",
		.password = "$avingLives1by1",
	},
	{
		.privilege = ADMIN, // remove completely or switch privelege to STAFF to test root user
		.name = "a",
		.password = "a",
	},
};
unsigned char user_count = 2;

// Define available rooms
struct Room rooms[ROOM_COUNT];

// Define available patients
struct Patient patients[MAX_PATIENT_COUNT];
unsigned char patient_count = 0;

// Generate fake data for testing purposes
// Can be switched over to loading data in from a file later
void generate_data(){
	for (int i=0; i<ROOM_COUNT; i++){
		rooms[i].id = i;
		if (i%10<5){
			patient_count;
			patients[patient_count].id = 200000+1000*(patient_count)+(rand()%1000); // formula for semi-random unique IDs
			strcpy(patients[patient_count].name,"Patient");
			// set last 2 characters to patient index
			patients[patient_count].name[7] = ((patient_count/10)%10) +'0';
			patients[patient_count].name[8] = (patient_count%10) +'0';
			patients[patient_count].name[9] = 0;
			patients[patient_count].status = (rand())%4;

			// print id and name for debug purposes
			printf("[ %u | %s\t%s\t ] Room: %d\n",
			patients[patient_count].id,patients[patient_count].name,PatientStatusToS[patients[patient_count].status],rooms[i].id);

			rooms[i].patient_id = patients[patient_count].id;
			rooms[i].status = FULL;
			patient_count++;
		}
		else{
			rooms[i].status = VACANT;
		}	
	}
}


//------------------------------------------------------------------------------------------------------
// Console Display Utilities


void title(char *string){
	puts("");
	puts(S_SEPARATOR);
	char title[50];
	strcpy(title,string);
	printf("[  %s  ]\n",strupr(title));
	puts("");
}

// defined prototype before declaration
unsigned char patient_room_index_from_id(unsigned int patient_id);

// Data Display 
void display_patient_data(unsigned char patient_index){
	struct Patient patient = patients[patient_index];
	char patient_room_index = patient_room_index_from_id(patient.id);

	puts("");
	puts(S_SEPARATOR);

	printf("Patient ID:\t%u\n"	, patient.id);
	printf("Name:\t\t%s\n"		, patient.name);
	printf("Status:\t\t%s\n"		, PatientStatusToS[patient.status]);
	if (patient.status!=DISMISSED)
		printf("room id:\t%d\n"		, rooms[patient_room_index].id);
	
	puts(S_SEPARATOR);
	puts("");
	
}


//------------------------------------------------------------------------------------------------------
// Console Input Utilities


// Common Functions
char* prompt_buffer(){
	// Used to buffer an input up to some amount of characters
	static char buffer[256]; // static to allow pointer return value
	printf("> ");
	fgets(buffer,256,stdin); // fgets to ensure no buffer overflow
	buffer[strcspn(buffer, "\n")] = 0; // switch linebreak character of buffer to terminator (common issue with fgets)
	return buffer;
}

void prompt_s(char *string){
	// Used to input up a string up to 256 characters long
	char *buffer = prompt_buffer();
	buffer[STRING_MAX_LEN-1] = 0; // set final character of string up to size to terminator
	memcpy(string,buffer,STRING_MAX_LEN); // copy data over to original string
}

int prompt_d(){
	// Used to input an integer value
	char *buffer = prompt_buffer();
	return atoi(buffer); // return int equivalent
}

char prompt_c(){
	// Used to input a single character
	char *buffer = prompt_buffer();
	return buffer[0]; // return only the first character
}

char prompt_y(){
	// Used to confirm decision
	char action = prompt_c();
	if (action != 'y' && action != 'Y') return 0;
	return 1;
}

// String Validation
char validate_string(char *string, char min_length, char max_length,char *valid_chars,char valid_num_allowed, char *valid_chars_text){

	// Check for invalid characters
	int index=0;
	while (string[index]!=0){ //check for null character
		if ((!isalpha(string[index]))
			&& (valid_num_allowed == 0 || !isdigit(string[index])) 
			&& (strchr(valid_chars,string[index]) == NULL)){ // if character not in valid characters
			printf("The input must only consist of %s\n",valid_chars_text);
			printf("The character [%c] is not allowed\n",string[index]);
			return 0;
		}
		index++;
	}

	// Check length
	if (index<min_length || index>max_length){
		printf("input must be %d to %d characters\n",min_length,max_length);
		return 0;
	}
	return 1;
}


//------------------------------------------------------------------------------------------------------
// Indexing Utility Functions


unsigned char room_index_from_id(unsigned char id){
	for (int i=0; i<ROOM_COUNT; i++){
		if (rooms[i].id==id) return i;
	}
	return UCHAR_MAX;
}

unsigned char patient_room_index_from_id(unsigned int patient_id){
	for (int i=0; i<ROOM_COUNT; i++){
		if (rooms[i].status == FULL && rooms[i].patient_id == patient_id) return i;
	}
	return UCHAR_MAX;
}

unsigned char patient_index_from_id(unsigned int patient_id){
	for (int i=0; i<patient_count; i++){
		if (patients[i].id == patient_id) return i;
	}
	return UCHAR_MAX;
}

unsigned int user_index_from_name(char *name){
	for (int i=0; i<user_count; i++){
		if (strcmp(name, users[i].name)==0){
			return i;
			break;
		}
	}
	return UINT_MAX;
}

//------------------------------------------------------------------------------------------------------
// Room Operations


void find_vacant_room(char *room_id,char *room_index){
	// pick a room
	while (0==0){
		title("room selection menu");
		puts("Please enter new room ID");
		*room_id = prompt_d();
		*room_index = room_index_from_id(*room_id);
		if (*room_index == UCHAR_MAX){
			puts("Incorrect room ID");
			puts("Reselect room? (y)");
			if (prompt_y()==1) continue;
			return;
		}
		if (rooms[*room_index].status==FULL){
			puts("Room currently full");
			puts("Reselect room? (y)");
			if (prompt_y()==1) continue;
			return;
		}
		break;
	}
}

void check_empty_rooms(){
	unsigned char empty_room_count = 0;
	unsigned char room_id = UCHAR_MAX; // assuming no more than 254 rooms
	for (int i=0; i<ROOM_COUNT; i++){
		if (rooms[i].status == VACANT){
			empty_room_count++;
			if (room_id == UCHAR_MAX)
				room_id = rooms[i].id;
		}
	}
	puts(S_SEPARATOR);
	if (empty_room_count == 0){
		puts("There are no empty rooms available");
		prompt_c();
		return;
	}
	printf("There are %d rooms available, the first of which is %d\n",empty_room_count,room_id);
	prompt_c();
}


//------------------------------------------------------------------------------------------------------
// Patient Operations


unsigned char patient_selection_loop(unsigned int *patient_id, unsigned char *patient_index){
	while (0 == 0){
		title("patient selection menu");
		puts("Please enter Patient ID:");
		*patient_id = prompt_d();

		// cancel if not found
		*patient_index = patient_index_from_id(*patient_id);
		if (*patient_index == UCHAR_MAX) break;

		display_patient_data(*patient_index);
		puts("Is this the correct patient? (y)");
		if (prompt_y() == 0) continue;

		return 1;
	}
	return 0;
}

void view_patient(){
	int patient_id;
	unsigned char patient_index;
	char success;

	// find patient
	while (success == 0){
		success = patient_selection_loop(&patient_id,&patient_index);
		puts("There is no patient with this ID, enter new ID? (y)");
		if (prompt_y()==0) return;
	}
	
}

char register_patient(unsigned int patient_id,unsigned char *patient_index){
	if (patient_count==MAX_PATIENT_COUNT){
		puts("Maximum number of patients was reached");
		return 0;
	} 
	struct Patient patient = {.id = patient_id, .status=DISMISSED};
	char success = 0;
	puts("");
	puts("Leave any field empty to exit");
	puts("Please enter new patient's name:");
	
	while (0==0){
		char last_name[50];
		puts(S_SEPARATOR);
		puts("First Name:");
		prompt_s(patient.name);

		if (strlen(patient.name)==0) return 0;//allow exit on empty prompt
		if (validate_string(
			patient.name,
			VALID_PATIENT_MIN_LEN,
			VALID_PATIENT_MAX_LEN,
			VALID_PATIENT_CHARS,
			VALID_PATIENT_NUM_ALLOWED,
			VALID_PATIENT_MESSAGE)==0) continue;

		puts("Last Name:");
		prompt_s(last_name);

		if (strlen(patient.name)==0) return 0;//allow exit on empty prompt
		if (validate_string(
			last_name,
			VALID_PATIENT_MIN_LEN,
			VALID_PATIENT_MAX_LEN,
			VALID_PATIENT_CHARS,
			VALID_PATIENT_NUM_ALLOWED,
			VALID_PATIENT_MESSAGE)==0) continue;
		
		strcat(patient.name," ");
		strcat(patient.name,last_name);

		printf("is patient name: [%s] acceptable? (y)\n",patient.name);
		if (prompt_y()==1){
			break;
		}
	}
	memcpy(&patients[patient_count],&patient,sizeof(patient));
	// increment patient count to add patient
	patient_count++;
	printf("Patient %s successfully created with id : %d\n",patient.name,patient.id);
	puts("");
	puts("Warning!:");
	puts("Patient Status can only be updated after admission to hospital");
	puts("To admit patient to hospital use transfer patient");
	return 1;
}

char transfer_patient(unsigned char passed_patient_index){
	int patient_id;
	unsigned char patient_room_index;
	unsigned char patient_index;
	unsigned char new_room_id;
	unsigned char new_room_index;
	char is_admission = 0, success;
	// Retreive patient
	if (passed_patient_index==UCHAR_MAX){
		// find patient
		success = patient_selection_loop(&patient_id,&patient_index);
		if (success == 0) return 0;
		patient_room_index = patient_room_index_from_id(patient_id);
	}
	else{
		// set to passed value
		success = 1;
		patient_index = passed_patient_index;
		patient_id = patients[patient_index].id;
	}
	
	// Allow admission of patient to hospital
	if (patient_room_index == UCHAR_MAX && is_admission == 0){
		puts(S_SEPARATOR);
		puts("Patient not currently in any room, admit patient to hospital? (y)");
		if (prompt_y()==1) is_admission = 1;
		else{
			puts(S_CANCELLED);
			return 0;
		}
		
	}
	
	while (0==0){
		find_vacant_room(&new_room_id,&new_room_index);
		
		// Confirm operation
		printf("Transfer patient %s to room %d ? (y)\n",
			patients[patient_index].name,new_room_id);
		if (prompt_y()==1){
			
			rooms[new_room_index].patient_id = patient_id;
			rooms[new_room_index].status = FULL;
			printf("patient %s successfully transfered to room %d\n",
			patients[patient_index].name,new_room_id);

			if (!is_admission){
				rooms[patient_room_index].patient_id = 0;
				rooms[patient_room_index].status = VACANT;
			}
			else{
				patients[patient_index].status = VISIT;
				puts("Remember to update patient status");
			}
			
			return 1;
		}
		puts("cancel operation? (y)");
		if (prompt_y()==1){
			puts(S_CANCELLED);
			break;
		}
		
	}
}

void update_patient(){
	unsigned int patient_id;
	unsigned char patient_room_index;
	unsigned char patient_index;

	while (0==0){
		char success = patient_selection_loop(&patient_id,&patient_index);

		if (success == 0){
			puts("Patient not registered, register a new patient? (y)");
			if (prompt_y()==1){
				success = register_patient(patient_id,&patient_index);
				if (success==0){
					puts(S_CANCELLED);
					return;
				}
				puts("Would you lik to admit patient? (y)");
				if (prompt_y()==1){
					if (transfer_patient(patient_id)) break;
					return;
				}
			}
			else{
				puts(S_CANCELLED);
				return;
			}
		}
		
		patient_room_index = patient_room_index_from_id(patient_id);
		
		// Allow re-entry of patient id
		if (patient_room_index == UCHAR_MAX){
			puts(S_SEPARATOR);
			puts("Patient not currently in any room, return? (y)");
			if (prompt_y()==0)continue;
			return;
		}
		break;
	}

	puts(S_SEPARATOR);
	puts("Enter New Status:");
	puts("(V) Visit\n(R) Recover\n(I) Ill\n(S) Severe\nOther to cancel");
	puts("");
	
	// convert char to enum
	const char* options = "vris";
	char *chr = strchr(options,tolower(prompt_c()));
	if (chr != NULL){
		patients[patient_index].status = (int)(chr-options);
		printf("patient status updated to %s\n",PatientStatusToS[patients[patient_index].status]);
		return;
	}
	puts(S_CANCELLED);
}

void discharge_patient(){
	int patient_id;
	unsigned char patient_room_index;
	unsigned char patient_index;
	// Uses find patient in room function to retreive patient
	while (0==0){
		char success = patient_selection_loop(&patient_id,&patient_index);

		if (success == 0) return;
		
		patient_room_index = patient_room_index_from_id(patient_id);
		
		// Allow re-entry of patient id
		if (patient_room_index == UCHAR_MAX){
			puts(S_SEPARATOR);
			puts("Patient not currently in any room, enter new id? (y)");
			if (prompt_y()==0)continue;
			puts(S_CANCELLED);
			return;
		}
		break;
	}
	
	// Clear the room's patient data
	rooms[patient_room_index].status = VACANT;
	rooms[patient_room_index].patient_id = 0;// not necessary but somewhat nice

	//change patient status
	patients[patient_index].status = DISMISSED; // special value for later use

	// Confirm operation success
	puts(S_SEPARATOR);
	printf("Patient %s has been successfully discharged from room %d\n",patients[patient_index].name,patient_room_index);
	prompt_c();
}

//------------------------------------------------------------------------------------------------------
// User Operations


void register_user(){
	if (user_count==MAX_USER_COUNT){
		puts("Maximum number of users was reached");
		return;
	} 
	struct User user;
	char pass[STRING_MAX_LEN];
	char success = 0;
	puts("");
	puts("Leave any field empty to exit");
	puts("Please enter new user's data:");

	// Username Loop
	while (0==0){
		puts(S_SEPARATOR);
		puts("Username:");
		prompt_s(user.name);

		if (strlen(user.name)==0) return;//allow exit on empty prompt
		if (validate_string(
			user.name,
			VALID_USERNAME_MIN_LEN,
			VALID_USERNAME_MAX_LEN,
			VALID_USERNAME_CHARS,
			VALID_USERNAME_NUM_ALLOWED,
			VALID_USERNAME_MESSAGE)==0) continue;

		//if name is already found ask for a new name
		if (user_index_from_name(user.name)!=UINT_MAX){
			puts("User name already in use");
			continue;
		}

		printf("is username: [%s] acceptable? (y)\n",user.name);
		if (prompt_y()==1){
			break;
		}
	}

	// Password Loop
	while (0==0){
		puts(S_SEPARATOR);
		puts("Password:");
		prompt_s(user.password);

		if (strlen(user.password)==0) return; //allow exit on empty prompt
		if (validate_string(
			user.password,
			VALID_PASSWORD_MIN_LEN,
			VALID_PASSWORD_MAX_LEN,
			VALID_PASSWORD_CHARS,
			VALID_PASSWORD_NUM_ALLOWED,
			VALID_PASSWORD_MESSAGE) == 0) continue; 

		puts("Re-enter Password:");
		prompt_s(pass);

		if (strlen(pass)==0) return; //allow exit on empty prompt
		if (strcmp(pass,user.password)!=0){
			puts("Password is incorrect");
			continue;
		}
		break;
	}
	
	//privelage loop
	success = 0;
	while (success==0){
		puts(S_SEPARATOR);
		puts("Privelage Level:");
		puts("(A) Admin");
		puts("(S) Staff");
		puts("");

		switch (tolower(prompt_c()))
		{
		case 'a':
			// warn user in case they choose to make an admin
			// should probably re-check for current admin's password
			// to avoid security risk of impersonation nand backdoor creation
			puts("Warning!!!: an admin would have the same privelage as yourself");
			puts("Are you sure you want to do this? (y)");
			if (prompt_y()==1){
				user.privilege=ADMIN;
				memcpy(&users[user_count],&user,sizeof(user));
				success = 1;
			}
			break;
		
		case 's':
			user.privilege=STAFF;
			memcpy(&users[user_count],&user,sizeof(user));
			success = 1;
			break;
		
		default:
			break;
		}
	}

	// increment user count to add user
	user_count++;
	printf("User %s successfully created with %s privileges\n",user.name,PrivsToS[user.privilege]);

}


//------------------------------------------------------------------------------------------------------
// Menu Loops


// session loop allows taking multiple actions wthin the same session
void session_loop(struct User user){
	char exit = 0;
	char action;
	while (exit == 0){
		title("main menu");
		puts("What would you like to do?");
		if (user.privilege == ADMIN)
			puts("(U) Register New User");
		puts("(C) Check Empty Rooms");
		if (user.privilege == ADMIN || user.privilege == STAFF){
			puts("(V) View Patient");
			puts("(S) Update Patient Status (or register patient)");
			puts("(T) Transfer Patient (or admit patient)");
			puts("(D) Discharge Patient");
		}
		puts("(E) Exit (Logout)");
		puts("");

		switch (tolower(prompt_c()))
		{
		case 'u':
			if (user.privilege != ADMIN) break;
			register_user();
			break;
		
		case 'c':
			check_empty_rooms();
			break;

		case 'v':
			if (user.privilege != ADMIN && user.privilege != STAFF) break;
			view_patient();
			break;
		
		case 's':
			if (user.privilege != ADMIN && user.privilege != STAFF) break;
			update_patient();
			break;
		
		case 't':
			if (user.privilege != ADMIN && user.privilege != STAFF) break;
			transfer_patient(UCHAR_MAX);
			break;
		
		case 'd':
			if (user.privilege != ADMIN && user.privilege != STAFF) break;
			discharge_patient();
			break;
		
		case 'e':
			exit = 1;
			break;
		default:
			break;
		}
	}
}

struct User login_attempt(){
	// Get login details
	// Note: taking all details at once to make security better
	char name[STRING_MAX_LEN];
	char password[STRING_MAX_LEN];
	title("sign-in menu");
	puts("Please enter login information:");
	puts("Username:");
	prompt_s(name);
	puts("Password:");
	prompt_s(password);
	puts(S_SEPARATOR);
	// loop over all available users
	for (int i=0; i<user_count; i++){
		// only login if both name and password are correct
		if (strcmp(users[i].name, name)==0 & strcmp(users[i].password, password)==0){
			printf("Succesfully logged in as %s\n",name);
			prompt_c();
			// return current user for further actions
			return users[i];
		}
	}
	// don't provide exact information about reason for refusal for security reasons
	printf("Failed to login as %s. Either the username or password is wrong\n",name);
	prompt_c();
	return NOT_A_USER;
}

// Login loop to allow multiple login attempts
struct User login_loop(){
	struct User user = NOT_A_USER;
	char action;
	char exit = 0;
	char admin_available = 0;

	// check if an admin is availble
	for (int i=0; i<user_count; i++){
		if (users[i].privilege == ADMIN){
			admin_available=1;
			break;
		}
	}

	if (admin_available ==0){
		title("login menu");
		puts("No admins currently available, signing in as root user");
		puts("Please setup an Admin as soon as possible to avoid security risks and allow regular logins");
		prompt_c();
		return ROOT_ADMIN;
	}
	
	while (user.privilege == NOPRV && exit == 0){
		title("login menu");
		puts("Welcome to Hospital Staff Login, what would you like to do?");
		puts("(G) Login as a Guest");
		puts("(U) Login as a User");
		puts("(E) Exit");
		puts("");
		switch (tolower(prompt_c()))
		{
		case 'g':
			puts("Succesfully logged in as a guest account");
			user = GUEST_USER;
			break;
		
		case 'u':
			user = login_attempt();
			break;
		
		case 'e':
			exit = 1;
			break;
		
		default:
			break;
		}
	}
	return user;
}
//------------------------------------------------------------------------------------------------------
// Code Entery


void main(){
	// generate fake data for testing. Should be replaced with load in from file
	generate_data();

	// main loop, allows for consequtive sessions
	struct User user;
	while (0==0){
		// login required to start session
		user = login_loop();
		if (user.privilege == NOPRV){
			// exit application if not logged in
			break;
		}
		// enter session loop
		session_loop(user);
	}
}