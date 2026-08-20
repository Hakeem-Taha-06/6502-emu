*= $0600

Start:
	LDX #$00
Loop:
	TXA
	ORA $FF
	STA $0200, X
	STA $0300, X
	STA $0400, X
	STA $0500, X
	CPX #$FF
	INX
	BNE Loop
	
End:
	JMP Start