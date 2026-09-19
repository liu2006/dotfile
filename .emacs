;; -*- lexical-binding: t; -*-

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
(setq-default whitespace-style
              '(face spaces space-mark))

(setq c-basic-offset 4
      cmake-tab-width 4
      lisp-body-indent 4)

(add-to-list 'default-frame-alist
             '(font . "CodeNewRomanNerdFont-12:regular"))

(use-package multiple-cursors
    :ensure t
    :bind (("C-S-c C-S-c" . mc/edit-lines)
           ("C->" . mc/mark-next-like-this)
           ("C-<" . mc/mark-previous-like-this)
           ("C-c C-<" . mc/mark-all-like-this)))

(use-package magit
    :ensure t
    :defer t)

(use-package vterm
    :ensure t
    :defer t
    :config
    (setq vterm-timer-delay 0.0084))

(use-package color-theme-sanityinc-tomorrow
    :ensure t
    :init (load-theme 'sanityinc-tomorrow-night t))

(use-package doom-modeline
    :ensure t
    :defer t
    :init (doom-modeline-mode))

(use-package smex
    :ensure t
    :defer t
    :bind (("M-x" . 'smex)
           ("M-X" . 'smex-major-mode-commands)))

(use-package lsp-mode
    :ensure t
    :hook
    ((c++-ts-mode
      c-ts-mode
      bash-ts-mode
      html-ts-mode
      json-ts-mod
      cmake-mode)
     . lsp-deferred)
    :commands (lsp lsp-deferred))

(use-package treesit
  :config
  (setq treesit-language-source-alist
        '((c "https://github.com/tree-sitter/tree-sitter-c")
          (cpp "https://github.com/tree-sitter/tree-sitter-cpp")
          (bash "https://github.com/tree-sitter/tree-sitter-bash")
          (html "https://github.com/tree-sitter/tree-sitter-html")
          (json "https://github.com/tree-sitter/tree-sitter-json")
          ))
  (setq treesit-font-lock-level 4)
  (add-to-list 'major-mode-remap-alist
               '(c-mode . c-ts-mode))
  (add-to-list 'major-mode-remap-alist
               '(c++-mode . c++-ts-mode))
  (add-to-list 'major-mode-remap-alist
               '(sh-mode . bash-ts-mode))
  (add-to-list 'major-mode-remap-alist
               '(mhtml-mode . html-ts-mode))
  (add-to-list 'major-mode-remap-alist
               '(json-mode . json-ts-mode))
  )

(setq smtpmail-smtp-server "smtp.gmail.com"
      smtpmail-smtp-service 587
      smtpmail-stream-type 'starttls)

(setq user-mail-address "liuhongshun80@gmail.com"
      user-full-name "pop")

(setq message-send-mail-function 'smtpmail-send-it)
(setq auth-sources '("~/.authinfo"))

(put 'dired-find-alternate-file 'disabled nil)
(put 'set-goal-column 'disabled nil)
(put 'upcase-region 'disabled nil)
