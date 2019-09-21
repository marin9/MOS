ENTRY(_start)
SECTIONS{
	. = 0x8000;

	.text : {
		obj/start.o;
		*(.text*);
	}

	.data : {
		*(.rodata*);
		*(.data*);
	}

	.bss : {
		*(.bss*);
	}
}
