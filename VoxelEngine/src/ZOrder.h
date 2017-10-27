#ifndef Z_ORDER_H
#define Z_ORDER_H

namespace Voxel
{
	/**
	*	@class ZOrder
	*	@brief Used in 2D rendering to sort the order of rendering.
	*
	*	ZOrder stores order by int. Anything that is lower than MIN_INT and MAX_INT won't work as expected.
	*	To compare two different z order, check global z order first, lower value to larger value.
	*	If global z order is same, check local z order, same rule as global z order.
	*/
	class ZOrder
	{
		friend struct ZOrderComp;
	private:
		// Global Z order
		int globalZOrder;
		// Local Z order
		int localZOrder;
		// True if initialized
		bool initialized;
	public:
		ZOrder() : globalZOrder(0), localZOrder(0), initialized(false) {}
		~ZOrder() = default;

		/**
		*	Get global z order
		*	@return A global z order
		*/
		int getGlobalZOrder() const { return globalZOrder; }

		/**
		*	Get local z order
		*	@return A local z order
		*/
		int getLocalZOrder() const { return localZOrder; }

		/**
		*	Check if this ZOrder is same.
		*	@param [in] other Other ZOrder to check
		*	@return True if same. Else, false.
		*/
		bool equals(const ZOrder& other) const
		{
			if (this->globalZOrder == other.globalZOrder)
			{
				return this->localZOrder == other.localZOrder;
			}
			else
			{
				return false;
			}
		}

		/**
		*	Check if this z order was initailized
		*	@return True if this ZOrder is initialized. Else, false.
		*/
		bool isInitialized() const { return this->initialized; }

		/**
		*	Sets z order.
		*	@param [in] global A global z order
		*	@param [in] local A local z order
		*/
		void setZOrder(const int global, const int local)
		{
			this->globalZOrder = global;
			this->localZOrder = local;
			this->initialized = true;
		}
	};

	// Comparator for ZORder
	struct ZOrderComp
	{
		bool operator()(const ZOrder& lhs, const ZOrder& rhs) const
		{
			if (lhs.globalZOrder == rhs.globalZOrder)
			{
				if (lhs.localZOrder == rhs.localZOrder)
				{
					return false;
				}
				else
				{
					return lhs.localZOrder < rhs.localZOrder;
				}
			}
			else
			{
				return lhs.globalZOrder < rhs.globalZOrder;
			}
		}
	};
}

#endif