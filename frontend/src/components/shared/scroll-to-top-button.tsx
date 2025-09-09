import { ArrowUp } from 'lucide-react'
import { useEffect, useState } from 'react'
import { Button } from '@/components/ui/button'
import { Tooltip, TooltipContent, TooltipTrigger } from '@/components/ui/tooltip'

function ScrollToTopButton() {
  const [visible, setVisible] = useState(false)

  function handleScroll() {
    setVisible(window.scrollY > 200)
  }

  function scrollToTop() {
    window.scrollTo({ top: 0, behavior: 'smooth' })
  }

  useEffect(() => {
    window.addEventListener('scroll', handleScroll)

    return () => {
      window.removeEventListener('scroll', handleScroll)
    }
  }, [])

  return (
    <Tooltip>
      <TooltipTrigger asChild>
        <Button
          size="icon"
          className={`fixed transition-opacity rounded-full shadow-lg z-1000 bottom-6 right-6 ${
            visible ? 'opacity-100' : 'opacity-0 pointer-events-none'
          }`}
          onClick={scrollToTop}
        >
          <ArrowUp className="w-5 h-5" />
        </Button>
      </TooltipTrigger>
      <TooltipContent>
        Scroll to top
      </TooltipContent>
    </Tooltip>
  )
}

export default ScrollToTopButton
