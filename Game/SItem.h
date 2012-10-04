//
// SItem.h
//

#ifndef SITEM_H
#define SITEM_H

#include <stdio.h>
#include <string>

namespace Sung
{

	class Item
	{
	public:
		Item ();
		Item (int);
		~Item ();

		inline int GetID () { return mID; }
		inline std::string GetName () { return mName; }
		inline std::string GetMesh () { return mMesh; }
		inline std::string GetICO () { return mICO; }
		inline int GetType() { return mType; }

		void SetID (int id);
		void SetName (std::string name);
		void SetMesh (std::string mesh);
		void SetICO (std::string ico);
		void SetType (int type);

		template<class Archive>
		void serialize(Archive &ar, const unsigned int) 
		{
			ar & mID;
			ar & mName;
			ar & mMesh;
			ar & mICO;
			ar & mType;
		}

	private:
		int mID;
		std::string mName;
		std::string mMesh;
		std::string mICO;
		int mType;
	};

}

#endif