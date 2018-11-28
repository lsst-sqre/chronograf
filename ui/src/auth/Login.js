import React from 'react'
import PropTypes from 'prop-types'

import Notifications from 'shared/components/Notifications'

import PageSpinner from 'src/shared/components/PageSpinner'
import SplashPage from 'shared/components/SplashPage'

const VERSION = process.env.npm_package_version

const Login = ({authData: {auth}}) => {
  if (auth.isAuthLoading) {
    return <PageSpinner />
  }

  return (
    <div>
      <Notifications />
      <SplashPage>
        <h1>SQuaSH</h1>
        <h4>A QC Service by LSST DM/SQuaRE</h4>
        <p>
        powered by
        </p>
        <h2>Chronograf</h2>
        <p>
          <strong>{VERSION}</strong> / Time-Series Data Visualization
        </p>
        {auth.links &&
          auth.links.map(({name, login, label}) => (
            <a key={name} className="btn btn-primary" href={login}>
              <span className={`icon ${name}`} />
              Log in with {label}
            </a>
          ))}
      </SplashPage>
    </div>
  )
}

const {array, bool, shape, string} = PropTypes

Login.propTypes = {
  authData: shape({
    me: shape(),
    links: array,
    isLoading: bool,
  }),
  location: shape({
    pathname: string,
  }),
}

export default Login
