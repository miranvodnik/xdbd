
#include <stddef.h>

#if defined(__cplusplus)
extern "C"
{
#endif

extern	void*	triggerLoad (int n, char* p[]);
extern	void	triggerFire (void* ctx, short function, unsigned char* oldData, unsigned char* newData, size_t size);
extern	void	triggerUnload (void* ctx);

#if defined(__cplusplus)
}
#endif
