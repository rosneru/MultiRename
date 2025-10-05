VERSION		EQU	1
REVISION	EQU	0

DATE	MACRO
		dc.b '5.10.2025'
		ENDM

VERS	MACRO
		dc.b 'MultiRename 1.0'
		ENDM

VSTRING	MACRO
		dc.b 'MultiRename 1.0 (5.10.2025)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: MultiRename 1.0 (5.10.2025)',0
		ENDM
