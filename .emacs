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
             '(font . "RecMonoCasualNerdFont-15:regular"))

;; (add-to-list 'default-frame-alist
;;              '(font . "CaskaydiaMonoNerdFont-14:light"))

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

(use-package doom-themes
    :ensure t
    :defer t
    :init
    (load-theme 'doom-one t)
    :custom
    (doom-themes-treemacs-theme "doom-atom")
    (doom-themes-enable-italic t) 
    :config
    (doom-themes-visual-bell-config)
    (doom-themes-neotree-config)
    (doom-themes-treemacs-config)
    (doom-themes-org-config))

(use-package doom-modeline
    :ensure t
    :defer t
    :init (doom-modeline-mode))
;; (use-package gruber-darker-theme
;;     :ensure t
;;     :defer t
;;     :init
;;     (load-theme 'gruber-darker t))

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
    :mode (("\\.clang-format\\'" . yaml-mode)
           ("\\.yaml\\'" . yaml-mode)))

(use-package cmake-mode
    :ensure t
    :defer t)

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

(use-package org-roam
    :ensure t
    :defer t
    :custom
    (org-roam-directory (file-truename "~/RoamNotes/"))
    :bind (("C-c n l" . org-roam-buffer-toggle)
           ("C-c n f" . org-roam-node-find)
           ("C-c n g" . org-roam-graph)
           ("C-c n i" . org-roam-node-insert)
           ("C-c n c" . org-roam-capture)
           ("C-c n j" . org-roam-dailies-capture-today))
    :config
    (setq org-roam-node-display-template (concat "${title:*} " (propertize "${tags:10}" 'face 'org-tag)))
    (org-roam-db-autosync-mode)
    (require 'org-roam-protocol))

(use-package ox-hugo
    :ensure t
    :defer t
    :after ox)

(use-package htmlize
    :ensure t
    :defer t)

(setq smtpmail-smtp-server "smtp.gmail.com"
      smtpmail-smtp-service 587
      smtpmail-stream-type 'starttls)

(setq user-mail-address "liuhongshun80@gmail.com"
      user-full-name "pop")

(setq message-send-mail-function 'smtpmail-send-it)
(setq auth-sources '("~/.authinfo"))

(setq c-basic-offset 4
      cmake-tab-width 4
      lisp-body-indent 4)

(put 'dired-find-alternate-file 'disabled nil)
(put 'set-goal-column 'disabled nil)
(put 'upcase-region 'disabled nil)
