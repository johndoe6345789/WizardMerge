import { Metadata } from 'next'
import './globals.css'

export const metadata: Metadata = {
  title: 'WizardMerge - Intelligent Merge Conflict Resolution',
  description: 'Resolve merge conflicts with intelligent dependency-aware algorithms',
}

export default function RootLayout({
  children,
}: {
  children: React.ReactNode
}) {
  return (
    <html lang="en">
      <body>{children}</body>
    </html>
  )
}
