/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

// Feature-specific components
import { Content, Title, Paragraph } from 'brave-ui/features/welcome'

// Utils
import { getLocale } from '../../../common/locale'

// Images
import { WelcomeShieldsImage } from 'brave-ui/features/welcome/images'

interface Props {
  index: number
  currentScreen: number
}

export default class ShieldsBox extends React.PureComponent<Props> {
  render () {
    const { index, currentScreen } = this.props
    return (
      <Content
        zIndex={index}
        active={index === currentScreen}
        screenPosition={'1' + (index + 1) + '0%'}
        isPrevious={index <= currentScreen}
      >
        <WelcomeShieldsImage />
        <Title>{getLocale('manageShields')}</Title>
        <Paragraph>{getLocale('adjustProtectionLevel')}</Paragraph>
      </Content>
    )
  }
}
