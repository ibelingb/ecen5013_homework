// array_sort.c
// ECEN5013 - HW3, prob4
// Date: 2/17/2019
// Author: Brian Ibeling
// About: Array sort method for an array of animal strings in Kernel Space

#include <linux/vmalloc.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>

#define ANIMAL_ARRAY_SIZE 50

struct Animal {
  char name[30];
  unsigned int count;
  struct list_head list;
};

//---------------------------------------------------------------------------
/* Static and local Variables */
// Filter parameters
static char * animal_type = ""; // Filter for animal type
static unsigned int count_greater_than = 0; // filter for count

/* Define modules parameters */
module_param(animal_type, charp, S_IRUGO);
module_param(count_greater_than, uint, S_IRUGO);

// Global variables
struct Animal animalList;
static unsigned int animalListSize = 0;

//---------------------------------------------------------------------------

/* Methods - kernel module */
static int __init animal_sort_init(void)
{
  // Local variables
  struct Animal* currentAnimal;
  struct Animal* newAnimal;
  struct Animal* animal;
  size_t i, j;

  printk(KERN_ALERT "Installing animal_sort module.");

  // Define animal seed array
  char* seedAnimalArray[] = { "frog", "frog", "wolf", "fox", "otter", 
                              "badger", "dragon", "ant", "wasp", "bee", 
                              "ox", "dragonfly", "fly", "bat", "fruitfly", 
                              "panda", "mouse", "rat", "dog", "cat", 
                              "bear", "bird", "lynx", "panther", "shark", 
                              "lizard", "lion", "duck", "cow", "pig", 
                              "gecko", "hippo", "tiger", "zebra", "ape", 
                              "giraffe", "monkey", "rhino", "elephant", "hampster", 
                              "snake", "whale", "fish", "starfish", "shrimp", 
                              "a", "b", "c", "d", "e" };

  // Print seed array
  printk(KERN_INFO "Received animal array to sort and print:\n");
  for(i=0; i<ANIMAL_ARRAY_SIZE; i++) {
    printk(KERN_INFO "seedAnimalArray[%d]: %s\n", i, seedAnimalArray[i]);
  }

  // Initialize linked list
  INIT_LIST_HEAD(&animalList.list);

  // Add first entry to list
  newAnimal = (struct Animal *)vmalloc(sizeof(struct Animal));
  strcpy(newAnimal->name, seedAnimalArray[0]);
  newAnimal->count = 1;
  list_add(&newAnimal->list, &animalList.list);
  animalListSize++;

  // Iterate through seedAnimalArray and load into linked list alphabetically
  for(i=1; i<ANIMAL_ARRAY_SIZE; i++) 
  {
    currentAnimal = &animalList;
    // Iterate through animalList, determine where new entry will be added
    list_for_each_entry(currentAnimal, &animalList.list, list)
    {
      //printk(KERN_INFO "strcmp output: %d | %s, %s", strcmp(seedAnimalArray[i], currentAnimal->name), seedAnimalArray[i], currentAnimal->name);

      // Add entry in front of this node
      if(strcmp(seedAnimalArray[i], currentAnimal->name) < 0)
      {
        // Allocate memory for new entry, load data, Add new animal to list
        newAnimal = (struct Animal *)vmalloc(sizeof(struct Animal));
        strcpy(newAnimal->name, seedAnimalArray[i]);
        newAnimal->count = 1;
        list_add_tail(&newAnimal->list, &currentAnimal->list);
        animalListSize++;
        break;

      }
      else if(strcmp(seedAnimalArray[i], currentAnimal->name) > 0)
      {
        // Allocate memory for new entry, load data, Add new animal to list
        // Check if entry is last in the list; if so, add to end of list
        if(list_is_last(&currentAnimal->list, &animalList.list) == true){
          // Allocate memory for new entry, load data, Add new animal to list
          newAnimal = (struct Animal *)vmalloc(sizeof(struct Animal));
          strcpy(newAnimal->name, seedAnimalArray[i]);
          newAnimal->count = 1;
          list_add(&newAnimal->list, &currentAnimal->list);
          animalListSize++;
          break;
        } else {
          struct Animal* nextAnimal = list_next_entry(currentAnimal, list);

          if(strcmp(seedAnimalArray[i], nextAnimal->name) < 0) {
            // Allocate memory for new entry, load data, Add new animal to list
            newAnimal = (struct Animal *)vmalloc(sizeof(struct Animal));
            strcpy(newAnimal->name, seedAnimalArray[i]);
            newAnimal->count = 1;
            list_add(&newAnimal->list, &currentAnimal->list);
            animalListSize++;
            break;
          } else {
            // continue
          }
        }
      }
      // duplicate found, increment count for entry in animalList
      else if(strcmp(currentAnimal->name, seedAnimalArray[i]) == 0)
      {
        currentAnimal->count++;
        printk(KERN_INFO "Duplicate animal found for {%s} with count {%d}\n", currentAnimal->name, currentAnimal->count);
        break;
      }
    }
  }

  // Print data for all linked list entries
  i = 0;
  printk(KERN_INFO "\n\n** Animal Array Sorted **\nLinked List AnimalArray with size {%d}\n", animalListSize);
  list_for_each_entry(animal, &animalList.list, list){
    printk(KERN_INFO "Linked List AnimalArray[%d]: {%s} with count {%d}\n", i, animal->name, animal->count);
    i++;
  }

  return 0;
}

static void __exit animal_sort_exit(void)
{
  struct Animal *animal, *tmp;
  // Remove each element from linked list, deallocate memory
  list_for_each_entry_safe(animal, tmp, &animalList.list, list){
    printk(KERN_INFO "Removing node entry {%s}, freeing memory.\n", animal->name);
    list_del(&animal->list);
    vfree(animal);
  }
  printk("Uninstalled animal_sort module\n");
}


//---------------------------------------------------------------------------
/* Methods */

//---------------------------------------------------------------------------

/* Module Initialization */
module_init(animal_sort_init);
module_exit(animal_sort_exit);
MODULE_DESCRIPTION("Kernel receives a string array of animals, and sorts them into kernel linked lists. linked list array information is then printed to the kernel log.");
MODULE_AUTHOR("Brian I.");
MODULE_LICENSE("GPL");
