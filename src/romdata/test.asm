;;; This source can be assembled with the Macroassembler AS
;;; (http://john.ccac.rwth-aachen.de:8000/as/)

;;; the following commands make test.bin
;;; asl test.asm
;;; p2bin test.p
	
        cpu 4004                ; AS's command to specify CPU

;;; Conditional jumps syntax for Macroassembler AS:
;;; JCN T     jump if TEST = 0 - most positive voltage or +5V
;;; JCN TN    jump if TEST = 1 - most negative voltage or -10V
;;; JCN C     jump if carry = 1
;;; JCN CN    jump if carry = 0
;;; JCN Z     jump if accumulator = 0
;;; JCN ZN    jump if accumulator != 0

P7 reg RERF

;;; ---------------------------------------------------------------------------
;;; Program Start
;;; ---------------------------------------------------------------------------

        org 0000H               ; beginning of 1702 EPROM

;;; ---------------------------------------------------------------------------
;;; Reset Entry
;;; ---------------------------------------------------------------------------
reset:
	FIM P7, 0
	SRC P7
	RDR
L1:	WMP
	JCN TN, L2		; jump if TEST == 1
	IAC
	IAC
L2:	DAC
	XCH R4
	JMS WAIT
	LD R4
	JUN L1

WAIT:	RDR			; wait 88.5ms*(port+1) (85ms...1.4s)
	XCH R3
L4:	ISZ R0, L4
	ISZ R1, L4
	ISZ R2, L4
	ISZ R3, L4
	BBL 0
	NOP
	NOP
	NOP
	NOP
	NOP

	END
	
