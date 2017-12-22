#ifndef Z_ORDER_H
#define Z_ORDER_H

namespace Voxel
{
	// Foward declaration
	namespace UI
	{
		class TransformNode;
		class Canvas;
	}

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
		// Friends
		friend struct ZOrderComp;
		friend class UI::TransformNode;
		friend class UI::Canvas;

	private:
		// Global Z order
		int globalZOrder;

		// Local Z order
		int localZOrder;
		
		/**
		*	Private constructor for firends.
		*	Only friends can create ZOrder instance with specific local order
		*	@param global Global z order
		*	@param local Local z order
		*/
		ZOrder(const int global, const int local) : globalZOrder(global), localZOrder(local) {}

	public:
		// constructor
		ZOrder() : globalZOrder(0), localZOrder(0) {}

		// constructor with global
		ZOrder(const int zOrder) : globalZOrder(zOrder), localZOrder(0) {}

		// default destructor
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
		*	Checks if global z order is MAX_INT
		*	return True if global z order is MAX_INT. Else, false.
		*/
		bool isGlobalZOrderMaxInt() const
		{
			return this->globalZOrder == (int)std::numeric_limits<int>::max();
		}

		/**
		*	Checks if local z order is MAX_INT
		*	return True if global z order is MAX_INT. Else, false.
		*/
		bool isLocalZOrderMaxInt() const
		{
			return this->localZOrder == (int)std::numeric_limits<int>::max();
		}
	};

	// Comparator for ZORder
	struct ZOrderComp
	{
		bool operator()(const ZOrder& lhs, const ZOrder& rhs) const
		{
			// Check global order first
			if (lhs.globalZOrder == rhs.globalZOrder)
			{
				// global is same. Check local
				if (lhs.localZOrder == rhs.localZOrder)
				{
					// local is same. 
					return false;
				}
				else
				{
					// compare local
					return lhs.localZOrder < rhs.localZOrder;
				}
			}
			else
			{
				// compare global
				return lhs.globalZOrder < rhs.globalZOrder;
			}
		}
	};
}

#endif