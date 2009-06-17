
#include <vehicle.h>

void
vehicle_accelerate_I (sc_object_t * o)
{
  sc_object_system_t *s = o->s;
  sc_void_function_t  oinmi;

  /* get the implementation of this method for this object */
  oinmi =
    sc_object_method_lookup (s, (sc_void_function_t) vehicle_accelerate_I,
                             (void *) o);

  /* cast object instance method implementation appropriately and call it */
  ((void (*)(sc_object_t *)) oinmi) (o);
}
