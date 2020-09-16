/* using .h as extension otherwise it generate the following error:

Error: apps/bleprph/src/shell_command.c:23:25: error: 'shell_set_cmd_struct' defined but not used [-Werror=unused-variable]
 static struct shell_cmd shell_set_cmd_struct = {
                         ^~~~~~~~~~~~~~~~~~~~
cc1: all warnings being treated as errors

*/

#include "console/console.h"

/* Application-specified header. */
#include "bleprph.h"
#include "shell/shell.h"
#include "nimble/mitm_params.h"



/** here is the shell functions

*/
// Command handler prototype declaration
static int shell_set_cmd(int argc, char **argv);
static int shell_del_cmd(int argc, char **argv);
static int shell_list_cmd(int argc, char **argv);

// Shell command struct

static const struct shell_cmd_help set_help = {
   .summary = "set a packet to modify during the mitm",
   .usage = "\tset [hexa of packet ll pdu] [hexa of new packet ll pdu] [hexa of the value]",
   .params = NULL,
};
static struct shell_cmd shell_set_cmd_struct = {
    .sc_cmd = "set",
    .sc_cmd_func = shell_set_cmd,
    .help = &set_help,
};

static const struct shell_cmd_help del_help = {
   .summary = "erase a packet from the modification list",
   .usage = "\tdel [opcode to delete] ",
   .params = NULL,
};
static struct shell_cmd shell_del_cmd_struct = {
    .sc_cmd = "del",
    .sc_cmd_func = shell_del_cmd,
    .help = &del_help,
};


static const struct shell_cmd_help list_help = {
   .summary = "list all the packet to mitmed",
   .usage = "\tlist ",
   .params = NULL,
};
static struct shell_cmd shell_list_cmd_struct = {
    .sc_cmd = "list",
    .sc_cmd_func = shell_list_cmd,
    .help = &list_help,
};


/*
Sameple of help function
#if MYNEWT_VAL(SHELL_CMD_HELP)
static const struct shell_param ticks_params[] = {
    {"on", "turn on"},
    {"off", "turn on"},
    { NULL, NULL}
};

static const struct shell_cmd_help ticks_help = {
   .summary = "shell ticks command",
   .usage = "usage: ticks [on|off]",
   .params = ticks_params,
};
#endif
*/



// set command handler
// allow use to set the packets we went to modify
static int
shell_set_cmd(int argc, char **argv)
{
//    int nb_of_modified_packets = sizeof(list_mitmed_packet);
    // command exemple : set feat 0
    if(argc == 3){

        /* If the two strings are the same st   vimrcmp return 0 */
        console_printf("atoi(argv[2]) : %x", atoi(argv[2]));
        if(!(strcmp(argv[1], "feat")) )
        {
            console_printf("\nWill set the features to %i\n", atoi(argv[2]) );
            mitm_params_local_features = atoi(argv[2]);
        }
        else
        {
            console_printf("Wrong var-name\n");
        }

//        console_printf("size of argv[0] %i\n", sizeof(argv[0]));
//        console_printf("value of argv[0] %s\n", argv[0]);
//        console_printf("size of argv[1] %i\n", sizeof(argv[1]));
//        console_printf("value of argv[1] %s\n", argv[1]);
//
//         console_printf("value atoi of argv[1] %i\n", atoi(argv[1]));

    }
    // wanted command : set 0c 0c 0931010c22   (for version ind packet)
    // : set [hexa of packet ll pdu] [hexa of new packet ll pdu] [hexa of the value]
    else if(argc == 4)
    {
        bool print_debug_info_list = 0;
        bool found_in_list = false;
        int current_index = nb_of_mitmed_packets;
        for (int hgg=0; hgg < nb_of_mitmed_packets ; hgg++)
        {
            if(print_debug_info_list) { console_printf("trying, %i ?= %i", (int)strtol(argv[1], NULL, 16), list_mitmed_packet[hgg].response_opcode); }
            // already existing in the list, need to update it
            if( (int)strtol(argv[1], NULL, 16) == list_mitmed_packet[hgg].response_opcode)
            {
                found_in_list = true;
                // means that the packets need to be modified
                current_index = hgg;
                // operation specific for
                if(print_debug_info_list) {  console_printf("Founded in the list\n"); }
            }
        }
        // need to append the list

        if(print_debug_info_list) { console_printf("This is the long string value : %s convertion = %lx\n", argv[3], (long)strtol(argv[3], NULL, 16)); }
        list_mitmed_packet[current_index].response_opcode = (int)strtol(argv[1], NULL, 16);
        list_mitmed_packet[current_index].response_new_opcode = (int)strtol(argv[2], NULL, 16);
//        list_mitmed_packet[current_index].datatorsp = (uint8_t)strtol(argv[3], NULL, 16);
        uint8_t test = 0;
        char testchar[2];
        bool odd_arg = false;
        // if string is not odd need to pretend (append at the begining) one 0
        if((strlen(argv[3]) % 2) != 0)
        {
            odd_arg= true;
        }
        for( int uhgg = 0; uhgg< strlen(argv[3]); uhgg++)
        {
            if(print_debug_info_list) { console_printf("bool ood activate :%i\n", odd_arg); }
            // manage if string as ood nb, handling odd uhgg
            if (odd_arg){
                if(print_debug_info_list) { console_printf("index lool%i, div %i\n", uhgg, ((uhgg+1)/2) ); }
                if (uhgg == 0){
                    testchar[0] = '0';
                    testchar[1] = argv[3][uhgg];
                }
                else{
                    testchar[0] = argv[3][uhgg];
                    testchar[1] = argv[3][uhgg+1];
                }

                list_mitmed_packet[current_index].datatorsp[((uhgg+1)/2)] = (uint8_t)strtol(testchar, NULL, 16);
                list_mitmed_packet[current_index].sizersp=((uhgg+1)/2)+1;

                if (uhgg != 0){
                    uhgg++;
                }
            }
            else
            {
                if(print_debug_info_list) { console_printf("index lool%i, div %i\n", uhgg, (uhgg/2) +1); }
                testchar[0] = argv[3][uhgg];
                testchar[1] = argv[3][uhgg+1];
                list_mitmed_packet[current_index].datatorsp[uhgg/2] = (uint8_t)strtol(testchar, NULL, 16);
                list_mitmed_packet[current_index].sizersp=(uhgg/2) +1;
                uhgg++;
            }
            test = (uint8_t)strtol(testchar, NULL, 16);
            if(print_debug_info_list) { console_printf("char not converted : %c%c \n", testchar[0], testchar[1]  );
            console_printf("converted to be stored: hex:%X, int:%i\n", test, test); }
        }

        if( found_in_list == false)
        {
            nb_of_mitmed_packets++;
        }
    }

    else
    {

        console_printf("Wrong command usage : %s [var-name] value\n", argv[0]);
        console_printf("List of var-name : \n");
        console_printf("- feat (int) : for setting features value p3013 from the 5.2 spec \n");
        console_printf("OR usage  %s [hexa of packet ll pdu] [hexa of new packet ll pdu] [hexa of the value] \n", argv[0]);
    }
//    console_printf("Test %i! with argc=%i and argv=%s\n", mitm_params_local_features, argc, *argv);
    return 0;
}

// list command handler
// list all the packet to mitmed
static int
shell_list_cmd(int argc, char **argv)
{
    // printing content of list_mitmed_packet
    for (int hgg=0; hgg < nb_of_mitmed_packets ; hgg++)
    {
        console_printf("printing mitmed packet nb %i : %2X , %2X, [",hgg,
        list_mitmed_packet[hgg].response_opcode ,
        list_mitmed_packet[hgg].response_new_opcode); //
        for( int uhgg = 0; uhgg< list_mitmed_packet[hgg].sizersp; uhgg++)
        {
            if(uhgg == 0){ console_printf("%02X",  list_mitmed_packet[hgg].datatorsp[uhgg]); }
            else{          console_printf(", %02X",list_mitmed_packet[hgg].datatorsp[uhgg]); }
        }
        console_printf("], %i \n", list_mitmed_packet[hgg].sizersp);
    }
    if (nb_of_mitmed_packets == 0 ) {
        console_printf("Not packet defined\n");
    }



    return 0;
}

// del command handler
// allow delete moficiation of a packet from the list
static int
shell_del_cmd(int argc, char **argv)
{
    if(argc == 2)
    {
        bool found_in_list = false;
        int index_to_delete = 0;
        bool print_debug_info_list = false;

        for (int hgg=0; hgg < nb_of_mitmed_packets ; hgg++)
        {
            // already existing in the list, need to update it
            if( (int)strtol(argv[1], NULL, 16) == list_mitmed_packet[hgg].response_opcode)
            {
                found_in_list = true;
                // means that the packets need to be modified
                index_to_delete = hgg;
                // operation specific for
                if(print_debug_info_list) {  console_printf("Founded in the list\n"); }

                // if not the last one
                if( hgg == nb_of_mitmed_packets-1){
                    if(print_debug_info_list) {  console_printf("Actually it is the last one just decreasing the nb\n"); }
                }
            }
            // if found and not the last one
            if(found_in_list && hgg != nb_of_mitmed_packets-1){
                list_mitmed_packet[hgg].response_opcode = list_mitmed_packet[hgg+1].response_opcode;
                list_mitmed_packet[hgg].response_new_opcode = list_mitmed_packet[hgg+1].response_new_opcode;
                for( int uhgg = 0; uhgg< list_mitmed_packet[hgg+1].sizersp; uhgg++)
                {
                    list_mitmed_packet[hgg].datatorsp[uhgg] = list_mitmed_packet[hgg+1].datatorsp[uhgg];
                }
                list_mitmed_packet[hgg].sizersp = list_mitmed_packet[hgg+1].sizersp;
            }
        }
        if (found_in_list){
            if(print_debug_info_list) { console_printf("fount it at %i\n", index_to_delete);}
            console_printf("deleted\n");
            nb_of_mitmed_packets--;
        }
        else
        {
            if(print_debug_info_list) {{console_printf("404 not found\n");}
        }
    }


    }
    else{
        console_printf("This command delete from the packet list to modify the right entry \n");
        console_printf("Wrong command usage : %s [opcode to delete] \n", argv[0]);

    }
    return 0;
}