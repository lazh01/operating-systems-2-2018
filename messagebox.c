#include "linux/kernel.h"
#include "linux/unistd.h"
#include "linux/slab.h"
#include <linux/uaccess.h>
#include <linux/errno.h>


typedef struct _msg_t msg_t;

struct _msg_t{
  msg_t* previous;
  int length;
  char* message;
};

static msg_t *bottom = NULL;
static msg_t *top = NULL;

/**
 * Adds the string from a buffer in user space, on top of the
 * on top of the stack as a new message.
 */
asmlinkage
int sys_messageboxput(char* buffer, int length){
    
    unsigned long flags;
    int count_uncopied;
    bool addr_valid;
    msg_t* msg;


    /**
     * checks if the length is above 0, if it is not 
     * returns an error for invalid argument:
     */
    if (length < 0){
        return -EINVAL;
    }
    
    //checks if it was able to allocate memory, if not returns 
    //out of memory error code
    msg = kmalloc(sizeof(msg_t), GFP_KERNEL);
    if (msg == NULL){
        return -ENOMEM;
    }

    msg->previous=NULL;
    msg->length=length;
    msg->message=kmalloc(length, GFP_KERNEL);
    

    //checks if it was able to allocate memory, if not returns 
    //out of memory error code, and deallocates previously allocated memory.
    if (msg->message == NULL){
        kfree(msg);
        return -ENOMEM;
    }

    /**
     * disables interrupts so that the kernel is not interrupted 
     * while accessing the top of the stack.
     */
    local_irq_save(flags);


    /**
     * Checks if the address is invalid, if it is the deallocates 
     * the created structure, and returns a bad address error code.
     */
    addr_valid = access_ok(VERIFY_READ, buffer, length);
    if(addr_valid == false){
        kfree(msg->message);
        kfree(msg);
        return -EFAULT;
    }


    /**
     * checks if any bytes remain uncopied from the message, 
     * returning a message size error if there is.
     */
    count_uncopied = copy_from_user(msg->message, buffer, length);
    if (count_uncopied>0){
        kfree(msg->message);
        kfree(msg);
        return -EMSGSIZE;
    }

    if(bottom == NULL){
        bottom=msg;
        top=msg;
    } else {
        msg->previous=top;
        top=msg;
    }

    local_irq_restore(flags);

    return 0;
}


/**
 * Removes and copies the message on top of the 
 * stack into a buffer in user-space.
 */
asmlinkage
int sys_messageboxget( char* buffer, int length ) {
    if(top != NULL){
        
        unsigned long flags;
        int count_uncopied;
        bool addr_valid;
        msg_t* msg;
        int mlength;
        
        /**
         * Disables interrupts until the program has finished
         * making changes to the stack.
         */
        local_irq_save(flags); 

        

        msg = top;
        mlength = msg->length;


        if (length-mlength<0){
            return -EINVAL;
        }

        /**
         * checks if the user-space address is invalid 
         * returning an error if it is.
         */
        addr_valid = access_ok(VERIFY_WRITE, buffer, mlength);
        if(addr_valid == false){
            return -EFAULT;
        }

        /**
         * Copies the message on top, returning an error if 
         * not all of the message could be copied.
         * The copy is done before the removal of msg from stack 
         * because it allows for checking before changing the state 
         * of the stack.
         */
        
        count_uncopied = copy_to_user(buffer, msg->message, mlength);
        if (count_uncopied>0){
            return -EMSGSIZE;
        }
        top=msg->previous;
        kfree(msg->message);
        kfree(msg);

        local_irq_restore(flags);

        return mlength;
    }
    return -1;
}

