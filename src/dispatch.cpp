void dispatch(PCB* p) {
    asm cli;

    asm {
        mov tsp, sp;
        mov tss, ss;
        mov tbp, bp;
    }

    running->sp = tsp;
    running->ss = tss;
    running->bp = tbp;

    running = p;

    tsp = running->sp;
    tss = running->ss;
    tbp = running->bp;

    asm {
        mov sp, tsp;
        mov ss, tss;
        mov bp, tbp;
        push ttbp;
        push ttds;
    }

    cout << "Dispatch finished." << endl;

    asm sti;
}

