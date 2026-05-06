all: brcm-gfx-buffer-client-header brcm-gfx-buffer-server-header brcm-gfx-buffer-code

brcm-gfx-buffer-client-header:
	$(SCANNER_TOOL) client-header < brcm-gfx-buffer.xml > brcm-gfx-buffer-client-protocol.h

brcm-gfx-buffer-server-header:
	$(SCANNER_TOOL) server-header < brcm-gfx-buffer.xml > brcm-gfx-buffer-server-protocol.h

brcm-gfx-buffer-code:
	$(SCANNER_TOOL) code < brcm-gfx-buffer.xml > brcm-gfx-buffer-protocol.c

clean:
	@rm -f brcm-gfx-buffer-client-protocol.h brcm-gfx-buffer-server-protocol.h brcm-gfx-buffer-protocol.c
