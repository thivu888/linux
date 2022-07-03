#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xd9726f80, "module_layout" },
	{ 0xe340d421, "device_destroy" },
	{ 0x37a0cba, "kfree" },
	{ 0x1bcee483, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xd5f10699, "cdev_add" },
	{ 0x4240b5cb, "cdev_init" },
	{ 0x54dc6ab9, "cdev_alloc" },
	{ 0x69ecc112, "kmem_cache_alloc_trace" },
	{ 0x36c11c94, "kmalloc_caches" },
	{ 0x64b60eb0, "class_destroy" },
	{ 0x9f4f34bc, "device_create" },
	{ 0xa946dcde, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "EE89C10D1BE3BD8D1EDF562");
