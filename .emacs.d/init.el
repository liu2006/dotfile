;; -*- lexical-binding: t; -*-
(require 'package)
(add-to-list 'package-archives '("melpa" . "https://melpa.org/packages/") t)
(setopt inhibit-startup-screen t)
(tool-bar-mode 0)
(menu-bar-mode 0)
(scroll-bar-mode 0)
(line-number-mode 1)
(column-number-mode 1)
(pixel-scroll-mode 1)
(setopt make-backup-files nil)
(global-display-line-numbers-mode 1)
(setopt display-line-numbers-type 'relative)
(setopt ring-bell-function 'ignore)
(setq show-paren-delay 0)
(electric-indent-mode 0)
(add-to-list 'default-frame-alist '(font . "Iosevka Light-18"))

(setopt c-ts-indent-offset 4)

(add-hook 'compilation-filter-hook
          #'ansi-color-compilation-filter)

(global-set-key (kbd "C-x C-b") #'ibuffer)

(setopt completion-styles '(flex basic))

(setopt completion-eager-display t
	completion-eager-update t)

(setopt ido-enable-flex-matching t)
(setopt ido-everywhere t)
(ido-mode 1)

(use-package smex
  :ensure t
  :bind
  (("M-x" . smex)
   ("M-X" . smex-major-mode-commands)
   ("C-c C-c M-x" . execute-extended-command)))

(use-package vterm
  :ensure t
  :config
  (setopt vterm-timer-delay nil))

(use-package nerd-icons
  :ensure t
  :config
  (setopt nerd-icons-font-family "Symbols Nerd Font Mono"))

(use-package doom-modeline
  :ensure t
  :init
  (doom-modeline-mode))

(use-package slang-mode
  :vc (:url "https://github.com/K1ngst0m/slang-mode.git")
  :mode "\\.slang\\'")

(use-package color-theme-sanityinc-tomorrow
  :ensure t
  :config
  (load-theme 'sanityinc-tomorrow-night t))

(use-package magit
  :ensure t)

(use-package multiple-cursors
  :ensure t
  :bind (("C-S-c C-S-c" . mc/edit-lines)
	 ("C->" . mc/mark-next-like-this)
	 ("C-<" . mc/mark-previous-like-this)
	 ("C-c C-<" . mc/mark-all-like-this)))

(setopt treesit-enabled-modes t
	treesit-auto-install-grammar 'always
	treesit-font-lock-level 4)

(use-package company
  :ensure t
  :hook
  (after-init-hook . global-company-mode)
  :config
  (setopt company-idle-delay nil)
  :bind
  ("M-TAB" . company-complete))

(custom-set-variables
 ;; custom-set-variables was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(package-selected-packages
   '(avy color-theme-sanityinc-tomorrow consult corfu doom-modeline f
	 lsp-mode magit multiple-cursors nerd-icons s shrink-path
	 slang-mode smex vterm))
 '(package-vc-selected-packages
   '((company-mode :url
		   "https://github.com/company-mode/company-mode.git")
     (smex :url "https://github.com/nonsequitur/smex.git")
     (color-theme-sanityinc-tomorrow :url
				     "https://github.com/purcell/color-theme-sanityinc-tomorrow.git")
     (slang-mode :url "https://github.com/K1ngst0m/slang-mode.git")
     (doom-modeline :url
		    "https://github.com/seagle0128/doom-modeline.git")
     (nerd-icons :url
		 "https://github.com/rainstormstudio/nerd-icons.el.git")
     (shrink-path :url
		  "https://github.com/emacsmirror/shrink-path.git")
     (s :url "https://github.com/magnars/s.el.git")
     (f :url "https://github.com/rejeep/f.el.git")
     (vterm :url "https://github.com/emacsmirror/vterm.git"))))
(custom-set-faces
 ;; custom-set-faces was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 )
