;; -*- lexical-binding: t; -*-
(require 'package)
(add-to-list 'package-archives '("melpa" . "https://melpa.org/packages/") t)

(setopt inhibit-startup-screen t)
(setopt tool-bar-mode nil)
(setopt menu-bar-mode nil)
(setopt	scroll-bar-mode nil)
(setopt	line-number-mode t)
(setopt	column-number-mode t)
(setopt	pixel-scroll-mode t)
(setopt	make-backup-files nil)
(setopt	global-display-line-numbers-mode t)
(setopt	display-line-numbers-type 'relative)
(setopt	ring-bell-function 'ignore)
(setopt	show-paren-delay 0)
(setopt	electric-indent-mode nil)

(add-to-list 'default-frame-alist '(font . "Iosevka Light-18"))

(setopt c-ts-indent-offset 4)

(add-hook 'compilation-filter-hook
          #'ansi-color-compilation-filter)

(global-set-key (kbd "C-x C-b") #'ibuffer)

(setopt completion-styles '(flex basic))

(setopt completion-eager-display t
	completion-eager-update t)

(setopt ido-enable-flex-matching t
	ido-everywhere t)

(setopt ido-mode 'both)

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

(add-hook 'auto-mode-alist
	  '("\\.xprofile\\'" . bash-ts-mode))

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
