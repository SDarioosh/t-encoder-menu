// The list of swipeable menus, in order. Add new pages here.
#include "pages.h"
#include "media_page.h"
#include "volume_page.h"

static VolumePage volume;
static MediaPage media;

Page *const PAGES[] = {
    &volume,
    &media,
};

const size_t PAGE_COUNT = sizeof(PAGES) / sizeof(PAGES[0]);
