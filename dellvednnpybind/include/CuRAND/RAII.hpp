#ifndef CURAND_RAII_HPP
#define CURAND_RAII_HPP

#include <functional>
#include <memory> // std::shared_ptr

#include "Status.hpp" // checkStatus

namespace CuRAND {
	template <typename T, Status (*C)(T*), Status (*D)(T)>
	class RAII {

		struct Resource {
			T object;

			Resource() {
				checkStatus(C(&object));
			}

			~Resource() {
				checkStatus(D(object));
			}
		};

  		std::shared_ptr<Resource> mResource;

  	public:

	  	RAII() : mResource(new Resource) {}

        T get() const {
            return mResource->object;
        }

	    operator T() const {
	      return mResource->object;
	    }
	};
}

#endif // CURAND_RAII_HPP
