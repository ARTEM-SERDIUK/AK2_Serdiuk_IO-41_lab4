// SPDX-License-Identifier: Dual BSD/GPL
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>
#include <linux/errno.h>

MODULE_AUTHOR("Artem Serdiuk");
MODULE_DESCRIPTION("Lab 4: Kernel module with list and timer events");
MODULE_LICENSE("Dual BSD/GPL");

static unsigned int count = 1;
module_param(count, uint, 0644);
MODULE_PARM_DESC(count, "Number of prints");

struct event_entry {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(event_list);

static int __init hello_init(void)
{
	unsigned int i;
	struct event_entry *entry;

	if (count > 10) {
		pr_err("hello: parameter count is too large: %u > 10\n", count);
		return -EINVAL;
	}

	if (count == 0 || (count >= 5 && count <= 10))
		pr_warn("hello: warning, unusual count value: %u\n", count);

	for (i = 0; i < count; i++) {
		entry = kmalloc(sizeof(*entry), GFP_KERNEL);
		if (!entry)
			return -ENOMEM;

		entry->time = ktime_get();
		list_add_tail(&entry->list, &event_list);
		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct event_entry *entry, *tmp;

	list_for_each_entry_safe(entry, tmp, &event_list, list) {
		pr_info("hello: event recorded at %lld ns\n",
			(long long)ktime_to_ns(entry->time));
		list_del(&entry->list);
		kfree(entry);
	}
}

module_init(hello_init);
module_exit(hello_exit);
