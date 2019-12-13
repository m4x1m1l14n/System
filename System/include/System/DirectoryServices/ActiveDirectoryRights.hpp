#pragma once

namespace System
{
	namespace DirectoryServices
	{
		enum class ActiveDirectoryRights
		{
			/// <summary>
			/// The right to get or set the SACL in the object security descriptor.
			/// </summary>
			AccessSystemSecurity = 16777216,
			/// <summary>
			/// The right to create children of the object.
			/// </summary>
			CreateChild = 1,
			/// <summary>
			/// The right to delete the object.
			/// </summary>
			Delete = 65536,
			/// <summary>
			/// The right to delete children of the object.
			/// </summary>
			DeleteChild = 2,
			/// <summary>
			/// The right to delete all children of this object,
			/// regardless of the permissions of the children.
			/// </summary>
			DeleteTree = 64,
			/// <summary>
			/// A customized control access right.
			/// For a list of possible extended rights, see the topic "Extended Rights" in the
			/// MSDN Library at http://msdn.microsoft.com. For more information about extended rights, see
			/// the topic "Control Access Rights" in the MSDN Library at http://msdn.microsoft.com.
			/// </summary>
			ExtendedRight = 256,
			/// <summary>
			/// The right to create or delete children, delete a subtree, read and write properties,
			/// examine children and the object itself, add and remove the
			/// object from the directory, and read or write with an extended right.
			/// </summary>
			GenericAll = 983551,
			/// <summary>
			/// The right to read permissions on, and list the contents of,
			/// a container object.
			/// </summary>
			GenericExecute = 131076,
			/// <summary>
			/// The right to read permissions on this object, read all the
			/// properties on this object, list this object name when
			/// the parent container is listed, and list the contents
			/// of this object if it is a container.
			/// </summary>
			GenericRead = 131220,
			/// <summary>
			/// The right to read permissions on this object, write all
			/// the properties on this object, and perform all validated
			/// writes to this object.
			/// </summary>
			GenericWrite = 131112,
			/// <summary>
			/// The right to list children of this object. For more
			/// information about this right, see the topic
			/// "Controlling Object Visibility" in the MSDN
			/// Library http://msdn.microsoft.com/library.
			/// </summary>
			ListChildren = 4,
			/// <summary>
			/// The right to list a particular object. For more information
			/// about this right, see the topic "Controlling Object Visibility"
			/// in the MSDN Library at http://msdn.microsoft.com/library.
			/// </summary>
			ListObject = 128,
			/// <summary>
			/// The right to read data from the security descriptor of
			/// the object, not including the data in the SACL.
			/// </summary>
			ReadControl = 131072,
			/// <summary>
			/// The right to read properties of the object.
			/// </summary>
			ReadProperty = 16,
			/// <summary>
			/// The right to perform an operation that is controlled by
			/// a validated write access right.
			/// </summary>
			Self = 8,
			/// <summary>
			/// The right to use the object for synchronization.
			/// This right enables a thread to wait until that object is
			/// in the signaled state.
			/// </summary>
			Synchronize = 1048576,
			/// <summary>
			/// The right to modify the DACL in the object security descriptor.
			/// </summary>
			WriteDacl = 262144,
			/// <summary>
			/// The right to assume ownership of the object. The user must be
			/// an object trustee. The user cannot transfer the ownership
			/// to other users.
			/// </summary>
			WriteOwner = 524288,
			/// <summary>
			/// The right to write properties of the object.
			/// </summary>
			WriteProperty = 32
		}
	}
}
