// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz

// This example uses a few C++ features that hopefully test whether the
// linkerscript and crt0 setup the C++ runtime correctly.

#include <ugba/ugba.h>

#include <new>

// ------------------------------------------------------------------

// Test virtual functions with derived classes

class base
{
public:
    base()
    {
        CON_Print("constructor: base class\n");
    }

    ~base()
    {
        CON_Print("destructor: base class\n");
    }

    virtual void print()
    {
        CON_Print("print: base class\n");
    }

    void show()
    {
        CON_Print("show: base class\n");
    }
};

class derived : public base
{
public:
    derived()
    {
        CON_Print("constructor: derived class\n");
    }

    ~derived()
    {
        CON_Print("destructor: derived class\n");
    }

    void print()
    {
        CON_Print("print: derived class\n");
    }

    void show()
    {
        CON_Print("show: derived class\n");
    }
};

void virtual_functions_test(void)
{
    base *bptr;
    derived d;
    bptr = &d;

    // virtual function, binded at runtime
    bptr->print();

    // Non-virtual function, binded at compile time
    bptr->show();
}

// ------------------------------------------------------------------

// Test allocation and deallocation with new and delete

void new_delete_test(void)
{
    char *string = new(std::nothrow) char[50];
    if (string == NULL)
        CON_Print("allocation: FAIL\n");
    delete[] string;

    CON_Print("allocation: OK\n");
}

// ------------------------------------------------------------------

// Test constructors that are called before main()

class global_initializer
{
public:
    global_initializer(void)
    {
        // This register is R/W, and it shouldn't affect the video output
        // because the windows aren't enabled.
        REG_WININ = 0x1234;
    }
};

global_initializer my_global_initializer;

void global_constructor_test(void)
{
    if (REG_WININ == 0x1234)
        CON_Print("global constructor OK\n");
    else
        CON_Print("global constructor FAIL\n");
}

// ------------------------------------------------------------------

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    // Run some C++ tests

    virtual_functions_test();
    global_constructor_test();
    new_delete_test();

    while (1)
        SWI_VBlankIntrWait();
}
