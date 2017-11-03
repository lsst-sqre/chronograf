import React, {PropTypes} from 'react'

import Authorized, {SUPERADMIN_ROLE} from 'src/auth/Authorized'

import Dropdown from 'shared/components/Dropdown'

import {USER_ROLES} from 'src/admin/constants/dummyUsers'
import {USERS_TABLE} from 'src/admin/constants/chronografTableSizing'

const OrgTableRow = ({
  user,
  organization,
  onToggleUserSelected,
  selectedUsers,
  isSameUser,
  onChangeUserRole,
}) => {
  const {colOrg, colRole, colSuperAdmin, colProvider, colScheme} = USERS_TABLE

  const isSelected = selectedUsers.find(u => isSameUser(user, u))

  const currentRole = user.roles.find(
    role => role.organization === organization.id
  )

  return (
    <tr className={isSelected ? 'selected' : null}>
      <td
        onClick={onToggleUserSelected(user)}
        className="chronograf-admin-table--check-col chronograf-admin-table--selectable"
      >
        <div className="user-checkbox" />
      </td>
      <td
        onClick={onToggleUserSelected(user)}
        className="chronograf-admin-table--selectable"
      >
        <strong>
          {user.name}
        </strong>
      </td>
      <td style={{width: colOrg}}>
        <span className="chronograf-user--org">
          {organization.name}
        </span>
      </td>
      <td style={{width: colRole}}>
        <span className="chronograf-user--role">
          <Dropdown
            items={USER_ROLES.map(r => ({
              ...r,
              text: r.name,
            }))}
            selected={currentRole.name}
            onChoose={onChangeUserRole(user, currentRole)}
            buttonColor="btn-primary"
            buttonSize="btn-xs"
            className="dropdown-80"
          />
        </span>
      </td>
      <Authorized requiredRole={SUPERADMIN_ROLE}>
        <td style={{width: colSuperAdmin}}>
          {user.superadmin ? 'yes' : '--'}
        </td>
      </Authorized>
      <td style={{width: colProvider}}>
        {user.provider}
      </td>
      <td className="text-right" style={{width: colScheme}}>
        {user.scheme}
      </td>
    </tr>
  )
}

const {arrayOf, func, shape, string} = PropTypes

OrgTableRow.propTypes = {
  user: shape(),
  organization: shape({
    name: string.isRequired,
    id: string.isRequired,
  }),
  onToggleUserSelected: func.isRequired,
  selectedUsers: arrayOf(shape()),
  isSameUser: func.isRequired,
  onSelectAddUserToOrg: func.isRequired,
  onChangeUserRole: func.isRequired,
}

export default OrgTableRow