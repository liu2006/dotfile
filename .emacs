(require 'package)
(add-to-list 'package-archives '("melpa" . "https://melpa.org/packages/") t)
(package-initialize)

(unless (file-exists-p "~/.emacs.d/backups")
    (make-directory "~/.emacs.d/backups" t))
(add-to-list 'backup-directory-alist (cons "." "~/.emacs.d/backups/"))

(setq custom-file "~/.emacs.d/.custom.el")
(unless (file-exists-p custom-file)
    (with-temp-buffer
        (write-file custom-file)))
(load custom-file)

(column-number-mode)
(tool-bar-mode 0)
(menu-bar-mode 0)
(scroll-bar-mode 0)
(ido-mode)
(ido-everywhere)
(setq ido-enable-flex-matching t)

(global-display-line-numbers-mode)
(setq display-line-numbers-type 'relative)
(setq inhibit-startup-screen t)
(setq-default tab-width 4)
(setq-default indent-tabs-mode nil)
(global-whitespace-mode)
(setq-default whitespace-style
              '(face spaces space-mark))

(add-to-list 'default-frame-alist
             '(font . "RecMonoCasualNerdFont-14:regular"))

(use-package multiple-cursors
  :ensure t
  :bind (("C-S-c C-S-c" . mc/edit-lines)
         ("C->" . mc/mark-next-like-this)
         ("C-<" . mc/mark-previous-like-this)
         ("C-c C-<" . mc/mark-all-like-this)))

(use-package magit
    :ensure t
    :defer t)

(use-package kdl-mode
    :ensure t
    :defer t
    :mode ("\\.kdl\\'"))

(use-package vterm
    :ensure t
    :defer t
    :config
    (setq vterm-timer-delay 0.0084)
    )

(use-package gruber-darker-theme
    :ensure t
    :defer t
    :init
    (load-theme 'gruber-darker t))

(use-package smex
    :ensure t
    :defer t
    :bind (("M-x" . 'smex)
           ("M-X" . 'smex-major-mode-commands)))

(use-package json-mode
    :ensure t
    :defer t
    :mode ("\\.jsonc\\'"))

(use-package yaml-mode
    :ensure t
    :defer t
    :mode ("\\.clang-format\\'")
    )

(use-package cmake-mode
    :ensure t
    :defer t
    )

(use-package slang-mode
    :load-path "/home/liu/vendored/slang-mode/"
    :defer t
    :mode ("\\.slang\\'"))

(use-package sh-mode
    :defer t
    :mode ("\\.xprofile\\'"))

(use-package c++-mode
    :defer t
    :mode ("\\.cppm\\'"))

(use-package eglot
    :ensure t
    :defer t
    :hook
    ((c++-mode c-mode) . eglot-ensure)
    :config
    (setq eglot-stay-out-of '(flymake))
    (add-to-list 'eglot-server-programs
                 '((c++-mode c-mode) . ("clangd"))))

(setq c-basic-offset 4)
(setq cmake-tab-width 4)
(setq lisp-body-indent 4)
