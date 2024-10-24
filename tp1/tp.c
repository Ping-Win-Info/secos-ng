/* GPLv2 (c) Airbus */
#include <debug.h>
#include <segmem.h>
#include <string.h>

void userland() {
   asm volatile ("mov %eax, %cr0");
}

void print_gdt_content(gdt_reg_t gdtr_ptr) {
    seg_desc_t* gdt_ptr;
    gdt_ptr = (seg_desc_t*)(gdtr_ptr.addr);
    int i=0;
    while ((uint32_t)gdt_ptr < ((gdtr_ptr.addr) + gdtr_ptr.limit)) {
        uint32_t start = gdt_ptr->base_3<<24 | gdt_ptr->base_2<<16 | gdt_ptr->base_1;
        uint32_t end;
        if (gdt_ptr->g) {
            end = start + ( (gdt_ptr->limit_2<<16 | gdt_ptr->limit_1) <<12) + 4095;
        } else {
            end = start + (gdt_ptr->limit_2<<16 | gdt_ptr->limit_1);
        }
        debug("%d ", i);
        debug("[0x%x ", start);
        debug("- 0x%x] ", end);
        debug("seg_t: 0x%x ", gdt_ptr->type);
        debug("desc_t: %d ", gdt_ptr->s);
        debug("priv: %d ", gdt_ptr->dpl);
        debug("present: %d ", gdt_ptr->p);
        debug("avl: %d ", gdt_ptr->avl);
        debug("longmode: %d ", gdt_ptr->l);
        debug("default: %d ", gdt_ptr->d);
        debug("gran: %d ", gdt_ptr->g);
        debug("\n");
        gdt_ptr++;
        i++;
    }
}

void print_seg_sel_content(seg_sel_t seg_sel) {
    debug("raw value : %d\t", seg_sel.raw);
    debug("rpl : %d\t", seg_sel.rpl);
    debug("ti : %d\t", seg_sel.ti);
    debug("index : %d\n", seg_sel.index);
}


void tp() {
	// Q2
    gdt_reg_t gdtr;
    get_gdtr(gdtr);
    print_gdt_content(gdtr);

    // Q3
    seg_sel_t cs,ds,ss,es,fs,gs;
    ss.raw = get_ss();
    ds.raw = get_ds();
    es.raw = get_es();
    fs.raw = get_fs();
    gs.raw = get_gs();
    cs.raw = get_seg_sel(cs);
    print_seg_sel_content(cs);
    print_seg_sel_content(ds);
    print_seg_sel_content(ss);
    print_seg_sel_content(es);
    print_seg_sel_content(fs);
    print_seg_sel_content(gs);

    // Q5 base address : Ox0040
    seg_desc_t flat_gdt[6];

    seg_desc_t null, data, code;
    null.raw = 0ULL;

    code.limit_1=0xffff;
    code.base_1=0;
    code.base_2=0;
    code.type=0xb; // could be 0xa too, doesn't really matter in flat mode
    code.s=1;
    code.dpl=0;
    code.p=1;
    code.limit_2=0xf;
    code.avl=1;
    code.l=0;
    code.d=1;
    code.g=1;
    code.base_3=0;

    data.limit_1=0xffff;
    data.base_1=0;
    data.base_2=0;
    data.type=0x3; // could be 0x2 too
    data.s=1;
    data.dpl=0;
    data.p=1;
    data.limit_2=0xf;
    data.avl=1;
    data.l=0;
    data.d=1;
    data.g=1;
    data.base_3=0;

    flat_gdt[0] = null;
    flat_gdt[1] = code;
    flat_gdt[2] = data;


    // Q6
    gdt_reg_t flat_gdtr; 
    flat_gdtr.desc = flat_gdt;
    flat_gdtr.limit = sizeof(flat_gdt) - 1;
    
    set_gdtr(flat_gdtr);
    set_cs(gdt_krn_seg_sel(1));
    set_ds(gdt_krn_seg_sel(2));

    // Q7
    get_gdtr(gdtr);
    print_gdt_content(gdtr);

    //Q8
    //set_cs(gdt_krn_seg_sel(2));
    //set_ds(gdt_krn_seg_sel(1));

    //Q9
    char  src[64];
    char *dst = 0;
    memset(src, 0xff, 64);
    _memcpy8(dst, src, 32);
    seg_desc_t es_desc;

    es_desc.limit_1=0x0020; // 32 B
    es_desc.base_1=0x0000;
    es_desc.base_2=0x60;
    es_desc.type=0x3; // could be 0x2 too
    es_desc.s=1;
    es_desc.dpl=0;
    es_desc.p=1;
    es_desc.limit_2=0x0;
    es_desc.avl=1;
    es_desc.l=0;
    es_desc.d=1;
    es_desc.g=0; // size in Bytes
    es_desc.base_3=0x00;

    flat_gdt[3] = es_desc;
    debug("GDT after adding 32B segment : \n");
    print_gdt_content(gdtr);

    // Q10
    seg_sel_t es_sel;
    es_sel.index = 3;
    es_sel.ti = 0;
    es_sel.rpl = 0;
    set_es(es_sel);
    _memcpy8(dst, src, 32);

    // Q11
    _memcpy8(dst, src, 64);

    // Q12
    code.dpl = 3;
    data.dpl = 3;
    flat_gdt[4] = code;
    flat_gdt[5] = data;

}
